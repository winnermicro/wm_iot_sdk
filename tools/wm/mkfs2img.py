import argparse
import datetime
import logging
import os
import posixpath
import random
import shutil
import subprocess
import sys
import tempfile
from datetime import timezone, timedelta
from pathlib import Path
from typing import Optional, Dict, Any, Tuple, Union
from pyfatfs.EightDotThree import EightDotThree
from pyfatfs.PyFat import PyFat
from pyfatfs.PyFatFS import PyFatFS

logger = logging.getLogger(__name__)

BEIJING_TIMEZONE = timezone(timedelta(hours=8))  # Beijing timezone UTC+8

class FSConfig:
    """Filesystem Configuration Constants"""

    SUPPORTED_FS = ['fatfs']

    FAT_TYPES = {
        'FAT12': 12,
        'FAT16': 16,
        'FAT32': 32
    }

    DEFAULT_CLUSTER_SIZES = {
        'FAT12': 4096,  # 4KB
        'FAT16': 8192,  # 8KB
        'FAT32': 16384  # 16KB
    }

    # Filesystem limits
    MAX_FILENAME_LENGTH = 255  # Maximum filename length
    MAX_PATH_LENGTH = 260      # Maximum path length
    MAX_DIR_DEPTH = 8         # Recommended maximum directory depth
    MAX_FAT32_SIZE = 128 * 1024 * 1024 * 1024  # FAT32 maximum size 128GB

    # Constants related to image header
    FLASH_BASE_ADDR = 0x8000000
    DEFAULT_IMAGE_HEADER = 0x20026000

class ImageManager:
    """Filesystem Image Manager"""

    def __init__(self, args):
        logger.info("Initializing image manager...")
        self.args = args
        self._setup_logging()
        self.fat = None
        logger.debug(f"Configuration parameters: {vars(args)}")

    def _setup_logging(self):
        """Configure logging level"""
        log_levels = {
            0: logging.ERROR,     # Show errors only
            1: logging.WARNING,   # Show warnings and errors
            2: logging.INFO,      # Show info, warnings and errors
            3: logging.DEBUG,     # Show debug info
        }
        level = log_levels.get(self.args.debug, logging.ERROR)
        log_format = '%(asctime)s [%(levelname).1s] %(message)s'

        logging.basicConfig(
            level=level,
            format=log_format,
            datefmt='%Y-%m-%d %H:%M:%S'
        )
        logger.debug(f"Log level set to: {logging.getLevelName(level)}")
        logger.debug(f"Log format: {log_format}")

    def _validate_path(self, path: str) -> bool:
        """Validate if path is legal"""
        try:
            logger.debug(f"Validating path: {path}")
            if len(path) > FSConfig.MAX_PATH_LENGTH:
                logger.error(f"Path length exceeds limit: {path}")
                return False

            if len(os.path.basename(path)) > FSConfig.MAX_FILENAME_LENGTH:
                logger.error(f"Filename length exceeds limit: {os.path.basename(path)}")
                return False

            depth = len(Path(path).parts)
            if depth > FSConfig.MAX_DIR_DEPTH:
                logger.warning(f"Directory depth({depth}) exceeds recommended value({FSConfig.MAX_DIR_DEPTH})")

            logger.debug(f"Path validation passed: {path}")
            return True
        except Exception as e:
            logger.error(f"Path validation failed: {e}")
            return False

    def _create_empty_image(self) -> bool:
        """Create empty image file"""
        try:
            logger.info("Starting to create empty image file...")
            # Validate size parameter
            if self.args.size <= 0:
                logger.error("Image size must be greater than 0")
                return False

            if self.args.size > FSConfig.MAX_FAT32_SIZE:
                logger.error("Image size exceeds FAT32 maximum supported size")
                return False

            # Check target path
            image_path = Path(self.args.image_file)
            logger.debug(f"Checking target path: {image_path}")
            if image_path.exists():
                if not os.access(image_path, os.W_OK):
                    logger.error("Target file has no write permission")
                    return False

            # Create temporary file
            logger.debug("Creating temporary file...")
            with tempfile.NamedTemporaryFile(delete=False) as temp_file:
                temp_file.seek(self.args.size - 1)
                temp_file.write(b'\0')
                logger.debug(f"Temporary file created: {temp_file.name}")

            # Verify temporary file size
            actual_size = os.path.getsize(temp_file.name)
            logger.debug(f"Verify temporary file size: Expected={self.args.size}, Actual={actual_size}")
            if actual_size != self.args.size:
                logger.error("Created image size is incorrect")
                os.unlink(temp_file.name)
                return False

            # Move to target location
            logger.debug(f"Moving temporary file to target location: {self.args.image_file}")
            shutil.move(temp_file.name, self.args.image_file)

            # Add final size check
            final_size = os.path.getsize(self.args.image_file)
            if final_size > self.args.size:
                logger.error(f"Generated image size ({final_size} bytes) exceeds specified size ({self.args.size} bytes)")
                os.unlink(self.args.image_file)
                return False

            logger.info(f"Empty image created successfully: {self.args.image_file}")
            return True

        except Exception as e:
            logger.error(f"Failed to create empty image: {e}")
            if 'temp_file' in locals():
                try:
                    logger.debug("Cleaning up temporary file...")
                    os.unlink(temp_file.name)
                except:
                    pass
            return False

    def _pack_fatfs(self) -> bool:
        """Pack file or directory into FAT image"""
        try:
            src_path = Path(self.args.create)
            logger.info(f"Starting to pack path: {src_path.resolve()}")
            logger.debug(f"Source path type: {'directory' if src_path.is_dir() else 'file'}")

            if not src_path.exists():
                logger.error(f"Source path does not exist: {src_path}")
                return False

            # Check if target file exists
            image_exists = Path(self.args.image_file).exists()
            if not image_exists:
                # Create new image file if it doesn't exist
                logger.info("Creating new empty image file...")
                if not self._create_empty_image():
                    return False

                # Format new image
                logger.info("Starting to format new image...")
                fat = PyFat()
                volume_id = random.randint(0, 0xFFFFFFFF)
                logger.debug(f"Generated volume ID: 0x{volume_id:08X}")

                # Select appropriate FAT type
                size_bytes = self.args.size
                if size_bytes <= 2 * 1024 * 1024:
                    fat_type = PyFat.FAT_TYPE_FAT12
                elif size_bytes <= 32 * 1024 * 1024:
                    fat_type = PyFat.FAT_TYPE_FAT16
                else:
                    fat_type = PyFat.FAT_TYPE_FAT32

                fat.mkfs(
                    filename=self.args.image_file,
                    fat_type=fat_type,
                    size=self.args.size,
                    sector_size=self.args.block,
                    number_of_fats=2,
                    label="WM_FATFS",
                    volume_id=volume_id,
                    media_type=0xF8
                )
                fat.close()
            else:
                logger.info("Using existing image file...")

            # Initialize filesystem for writing files
            logger.debug("Initializing filesystem for writing files...")
            self.fatfs = PyFatFS(
                filename=self.args.image_file,
                encoding='utf-8',
                preserve_case=True
            )

            def add_file_to_image(src_file: Path, fat_path: str):
                """Add single file to image"""
                try:
                    if not self._validate_path(fat_path):
                        raise ValueError(f"Invalid file path: {fat_path}")

                    # Create parent directory
                    parent_dir = posixpath.dirname(fat_path)
                    if parent_dir != '/':
                        try:
                            self.fatfs.makedir(parent_dir, recreate=True)
                            logger.debug(f"Created parent directory: {parent_dir}")
                        except Exception as e:
                            logger.warning(f"Error creating directory {parent_dir}: {e}")

                    # Write file content
                    file_size = src_file.stat().st_size
                    logger.info(f"Starting to write file: {fat_path} (size: {file_size:,} bytes)")
                    with src_file.open('rb') as src:
                        with self.fatfs.openbin(fat_path, 'wb') as dst:
                            shutil.copyfileobj(src, dst)

                    logger.info(f"Successfully wrote file: {fat_path}")

                except Exception as e:
                    logger.error(f"Failed to add file {src_file}: {str(e)}")
                    logger.debug("Error details:", exc_info=True)
                    raise

            def add_directory_to_image(src_path: Path, fat_path: str = '/'):
                """Add directory and its contents to image"""
                logger.debug(f"Starting to add directory: {src_path} -> {fat_path}")

                if not self._validate_path(fat_path):
                    raise ValueError(f"Invalid directory path: {fat_path}")

                # Create directory (except root)
                if fat_path != '/':
                    try:
                        self.fatfs.makedir(fat_path, recreate=True)
                        logger.debug(f"Created directory: {fat_path}")
                    except Exception as e:
                        logger.debug(f"Error creating directory {fat_path}: {e}")

                # Add directory contents
                for item in src_path.iterdir():
                    item_fat_path = posixpath.join(fat_path, item.name)
                    item_src_path = src_path / item.name

                    if item.is_file():
                        logger.debug(f"Adding file: {item_src_path} -> {item_fat_path}")
                        add_file_to_image(item_src_path, item_fat_path)
                    elif item.is_dir():
                        logger.debug(f"Adding directory: {item_src_path} -> {item_fat_path}")
                        add_directory_to_image(item_src_path, item_fat_path)

            if src_path.is_file():
                add_file_to_image(src_path, f"/{src_path.name}")
            elif src_path.is_dir():
                add_directory_to_image(src_path, '/')
            else:
                logger.error(f"Unsupported file type: {src_path}")
                return False

            # Add size check after packing
            final_size = os.path.getsize(self.args.image_file)
            if final_size > self.args.size:
                logger.error(f"Final image size ({final_size} bytes) exceeds specified size ({self.args.size} bytes)")
                return False

            logger.info(f"Successfully {'added to' if image_exists else 'created'} image: {self.args.image_file}")
            logger.info(f"Final image size: {final_size} bytes")
            return True

        except Exception as e:
            logger.error(f"Packing failed: {str(e)}")
            logger.debug("Error details:", exc_info=True)
            return False
        finally:
            if hasattr(self, 'fatfs'):
                try:
                    logger.debug("Closing filesystem...")
                    self.fatfs.close()
                except:
                    pass

    def _unpack_fatfs(self) -> bool:
        """Unpack FAT image to directory"""
        try:
            dest_path = Path(self.args.unpack)
            logger.info(f"Starting to unpack image to directory: {dest_path}")
            if not dest_path.exists():
                logger.debug(f"Creating target directory: {dest_path}")
                dest_path.mkdir(parents=True)
            elif not os.access(dest_path, os.W_OK):
                logger.error(f"Target directory has no write permission: {dest_path}")
                return False

            # Initialize PyFatFS
            logger.debug("Initializing filesystem...")
            self.fatfs = PyFatFS(
                filename=self.args.image_file,
                encoding='utf-8',
                preserve_case=True
            )

            def extract_from_image(fat_path: str = '/', dest_dir: Path = dest_path):
                try:
                    if not self._validate_path(fat_path):
                        raise ValueError(f"Invalid path: {fat_path}")

                    # Get directory contents
                    logger.debug(f"Reading directory contents: {fat_path}")
                    entries = self.fatfs.listdir(fat_path)

                    for entry in entries:
                        entry_path = posixpath.join(fat_path, entry)
                        dst_path = dest_dir / entry

                        # Check if it's a directory
                        if self.fatfs.getinfo(entry_path).is_dir:
                            logger.debug(f"Creating directory: {dst_path}")
                            dst_path.mkdir(exist_ok=True)
                            extract_from_image(entry_path, dst_path)
                        else:
                            # Copy file contents
                            logger.info(f"Unpacking file: {entry_path} -> {dst_path}")
                            with self.fatfs.openbin(entry_path, 'rb') as src:
                                with dst_path.open('wb') as dst:
                                    shutil.copyfileobj(src, dst)

                except Exception as e:
                    logger.error(f"Failed to unpack {fat_path}: {str(e)}")
                    raise

            extract_from_image()
            logger.info(f"Successfully unpacked image to directory {self.args.unpack}")
            return True

        except Exception as e:
            logger.error(f"Unpacking failed: {str(e)}")
            return False
        finally:
            if hasattr(self, 'fatfs'):
                try:
                    logger.debug("Closing filesystem...")
                    self.fatfs.close()
                except:
                    pass

    def _list_fatfs(self) -> bool:
        """List files and directory structure in FAT image"""
        try:
            # Initialize PyFatFS
            logger.debug("Initializing filesystem...")
            self.fatfs = PyFatFS(
                filename=self.args.image_file,
                encoding='utf-8',
                preserve_case=True
            )

            print("            .")  # Root directory

            def format_size(size: int) -> str:
                """Format file size display"""
                if size < 1024:
                    return f"{size}B"
                elif size < 1024 * 1024:
                    return f"{size/1024:.1f}K" 
                else:
                    return f"{size/(1024*1024):.1f}M"

            def list_directory(path: str = '/', prefix: str = ""):
                """Recursively list directory contents"""
                if not self._validate_path(path):
                    logger.warning(f"Skipping invalid path: {path}")
                    return

                logger.debug(f"Reading directory: {path}")
                entries = self.fatfs.listdir(path)
                entries = [e for e in entries if e not in ['.', '..']]
                entries.sort(key=str.lower)

                count = len(entries)
                for i, entry in enumerate(entries):
                    is_last = i == count - 1
                    current_prefix = prefix + ("└── " if is_last else "├── ")
                    next_prefix = prefix + ("    " if is_last else "│   ")

                    entry_path = posixpath.join(path, entry)
                    info = self.fatfs.getinfo(entry_path)

                    if info.is_dir:
                        print(f"            {current_prefix}{entry}/")
                        try:
                            list_directory(entry_path, next_prefix)
                        except Exception as e:
                            logger.error(f"Failed to access directory {entry_path}: {str(e)}")
                    else:
                        try:
                            size_str = format_size(info.size)
                            mtime = int(info.modified.timestamp())
                            mtime = datetime.datetime.fromtimestamp(mtime, BEIJING_TIMEZONE).strftime("%Y-%m-%d %H:%M:%S")
                            print(f"            {current_prefix}{entry} ({size_str}, {mtime})")
                        except Exception as e:
                            logger.error(f"Failed to get file info for {entry_path}: {str(e)}")

            list_directory()
            return True

        except Exception as e:
            logger.error(f"Failed to list files: {str(e)}")
            return False
        finally:
            if self.fat:
                try:
                    logger.debug("Closing filesystem...")
                    self.fat.close()
                except:
                    pass

    def _visualize_fatfs(self) -> bool:
        """Visualize FAT image"""
        try:
            logger.info("Starting filesystem visualization...")
            self.fat = PyFat()
            self.fat.open(self.args.image_file, read_only=True)

            print("\nFAT Filesystem Information:")
            print("-" * 40)
            print(f"Filesystem Type: FAT{self.fat.fat_type}")
            print(f"Bytes/Sector: {self.fat.bpb_header['BPB_BytsPerSec']}")
            print(f"Sectors/Cluster: {self.fat.bpb_header['BPB_SecPerClus']}")
            print(f"Reserved Sectors: {self.fat.bpb_header['BPB_RsvdSecCnt']}")
            print(f"Number of FATs: {self.fat.bpb_header['BPB_NumFATs']}")
            print(f"Root Directory Entries: {self.fat.bpb_header['BPB_RootEntCnt']}")
            print(f"Sectors per FAT: {self.fat._fat_size}")
            print(f"Total Sectors: {self.fat._get_total_sectors()}")
            print(f"Bytes per Cluster: {self.fat.bytes_per_cluster}")
            print(f"First Data Sector: {self.fat.first_data_sector}")

            # Calculate filesystem size
            total_bytes = self.fat._get_total_sectors() * self.fat.bpb_header['BPB_BytsPerSec']
            if total_bytes < 1024 * 1024:  # < 1MB
                print(f"Total Filesystem Size: {total_bytes/1024:.2f} KB")
            elif total_bytes < 1024 * 1024 * 1024:  # < 1GB
                print(f"Total Filesystem Size: {total_bytes/1024/1024:.2f} MB")
            else:
                print(f"Total Filesystem Size: {total_bytes/1024/1024/1024:.2f} GB")

            # Display volume label
            print(f"Volume Label: {self.fat.bpb_header['BS_VolLab'].decode().strip()}")

            # Display filesystem status
            print(f"Filesystem Status: {'Dirty' if self.fat._is_dirty() else 'Clean'}")

            # Display OEM name
            print(f"OEM Name: {self.fat.bpb_header['BS_OEMName'].decode().strip()}")

            # If FAT32, display FSInfo information
            if self.fat.fat_type == PyFat.FAT_TYPE_FAT32:
                print("\nFAT32 Specific Information:")
                print(f"FSInfo Sector: {self.fat.bpb_header['BPB_FSInfo']}")
                print(f"Backup Boot Sector: {self.fat.bpb_header['BPB_BkBootSec']}")
                print(f"Root Directory Cluster: {self.fat.bpb_header['BPB_RootClus']}")

            print("-" * 40)
            logger.info("Filesystem visualization complete")
            return True

        except Exception as e:
            logger.error(f"Visualization failed: {e}")
            return False
        finally:
            if self.fat:
                try:
                    logger.debug("Closing filesystem...")
                    self.fat.close()
                except:
                    pass

    def _add_image_header(self, fs_image_path: str) -> bool:
        """Add image header to filesystem image"""
        try:
            # Check if run-address parameter is provided
            if not self.args.run_address:
                logger.info("Skipping image header generation: run-address not provided")
                return True

            logger.info("Adding image header to filesystem image...")

            # Get script directory
            script_dir = os.path.dirname(__file__)
            bin2img_tool = os.path.join(script_dir, "bin2img.py")

            if not os.path.exists(bin2img_tool):
                logger.error(f"bin2img.py tool not found at: {bin2img_tool}")
                return False

            # Generate path for .img file with header
            img_output = os.path.splitext(fs_image_path)[0] + '.img'

            # 将run_address转换为16进制字符串
            run_address_hex = f"0x{self.args.run_address:08x}"
            # Build command
            command = [
                "python", bin2img_tool,
                "--input-binary", fs_image_path,
                "--output-name", img_output,
                "--compress-type", "0",
                "--image-type", "2", 
                "--image-header", "0x20026000",
                "--run-address", run_address_hex,
                "--update-address", "0",
                "--next-image-header", "0",
                "--upd-no", "0"
            ]

            logger.debug(f"Running command: {' '.join(command)}")
            subprocess.run(command, check=True)

            logger.info(f"Successfully created image with header: {img_output}")
            return True

        except subprocess.CalledProcessError as e:
            logger.error(f"Failed to add image header: {e}")
            return False
        # except Exception as e:
        #     logger.error(f"Unexpected error while adding image header: {e}")
        #     return False

    def pack_directory(self) -> bool:
        """Pack directory into filesystem image"""
        if self.args.type != 'fatfs':
            logger.error("Currently only fatfs type is supported")
            return False

        # Create filesystem image first
        if not self._pack_fatfs():
            return False

        # Add image header
        return self._add_image_header(self.args.image_file)

    def unpack_image(self) -> bool:
        """Unpack filesystem image to directory"""
        if self.args.type != 'fatfs':
            logger.error("Currently only fatfs type is supported")
            return False
        return self._unpack_fatfs()

    def list_contents(self) -> bool:
        """List files in image"""
        if self.args.type != 'fatfs':
            logger.error("Currently only fatfs type is supported")
            return False
        return self._list_fatfs()

    def visualize(self) -> bool:
        """Visualize filesystem image"""
        if self.args.type != 'fatfs':
            logger.error("Currently only fatfs type is supported")
            return False
        return self._visualize_fatfs()

def parse_arguments() -> argparse.Namespace:
    """Parse command line arguments"""
    logger.debug("Starting to parse command line arguments...")
    parser = argparse.ArgumentParser(
        description='Create/manipulate filesystem images',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='''
Examples:
  # Create a 1MB FAT filesystem image and pack fatfs_folder directory into it
  %(prog)s -c fatfs_folder -s 1048576 -t fatfs fatfs.bin

  # Create a 2MB FAT filesystem image with specified run address
  %(prog)s -c fatfs_folder -s 0x200000 -t fatfs -r 0x8010000 fatfs.bin

  # Unpack FAT filesystem image to specified directory
  %(prog)s -u output_dir -t fatfs fatfs.bin

  # List files in FAT filesystem image
  %(prog)s -l -t fatfs fatfs.bin

  # Show detailed information of FAT filesystem image
  %(prog)s -i -t fatfs fatfs.bin
        '''
    )

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-c', '--create', metavar='pack_dir', help='create fs image from a directory')
    group.add_argument('-u', '--unpack', metavar='dest_dir', help='unpack fs image to a directory')
    group.add_argument('-l', '--list', action='store_true', help='list files in fs image')
    group.add_argument('-i', '--visualize', action='store_true', help='visualize fs image')

    # Other arguments
    parser.add_argument('-d', '--debug', type=int, choices=range(4), default=2,
                      help='Debug level. 0 means no debug output')
    parser.add_argument('-b', '--block', type=int, default=512, help='fs block size, in bytes')
    parser.add_argument('-p', '--page', type=int, default=512, help='fs page size, in bytes')
    parser.add_argument('-s', '--size', type=str, help='fs image size, in bytes')
    parser.add_argument('-t', '--type', choices=FSConfig.SUPPORTED_FS, default='fatfs',
                      help='fs type, support fatfs (littlefs support coming soon)')
    parser.add_argument('-r', '--run-address', metavar='addr',
                      help='run address in hex (e.g. 0x8000000)')
    parser.add_argument('image_file', help='filesystem image file (e.g. fatfs.bin)')

    args = parser.parse_args()

    # Size parameter is only required when creating image
    if args.create and not args.size:
        parser.error('the following arguments are required: -s/--size')

    # Handle size parameter
    if args.size:
        try:
            if args.size.startswith('0x'):
                args.size = int(args.size, 16)
            else:
                args.size = int(args.size)

            if args.size <= 0:
                parser.error('Size must be positive')
            if args.size > FSConfig.MAX_FAT32_SIZE:
                parser.error('Size too large for FAT32')
        except ValueError:
            parser.error('Invalid size value')

    # Handle run address parameter
    if args.run_address:
        try:
            if args.run_address.startswith('0x'):
                args.run_address = int(args.run_address, 16)
            else:
                args.run_address = int(args.run_address)

            if args.run_address <= 0:
                parser.error('Run address must be positive')
            if args.run_address < FSConfig.FLASH_BASE_ADDR:
                parser.error(f'Run address must be greater than or equal to {hex(FSConfig.FLASH_BASE_ADDR)}')
        except ValueError:
            parser.error('Invalid run address value')

    logger.debug(f"Arguments parsing complete: {vars(args)}")
    return args

def main():
    """Main function"""
    try:
        logger.info("Program execution started...")
        args = parse_arguments()
        manager = ImageManager(args)

        if args.create:
            logger.info(f"Starting to pack directory: {args.create}")
            if not manager.pack_directory():
                sys.exit(1)
        elif args.unpack:
            logger.info(f"Starting to unpack image to: {args.unpack}")
            if not manager.unpack_image():
                sys.exit(1)
        elif args.list:
            logger.info("Starting to list image contents...")
            if not manager.list_contents():
                sys.exit(1)
        elif args.visualize:
            logger.info("Starting to visualize image...")
            if not manager.visualize():
                sys.exit(1)

    except KeyboardInterrupt:
        logger.info("\nOperation cancelled by user")
        sys.exit(1)
    except Exception as e:
        logger.error(f"Unexpected error: {e}", exc_info=True)
        sys.exit(1)

if __name__ == '__main__':
    main()
