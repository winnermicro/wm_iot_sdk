#/usr/bin/python
import os
import sys
import glob
import shutil
import zipfile
import platform

args = sys.argv

version_default  = "latest"
version_specific = "specific"

if (len(args) == 1):
    # all
    cmd = "-d"
else:
    cmd = "-p"

soc_dict = {
    "w800" : "w800",
#    "w801s": "w801s",
#    "w803" : "w803",
}

language_dict = {
    "zh_CN": "zh_CN",
    "en"   : "en"
}

type_dict = {
    "html": "html",
    "pdf" : "latexpdf"
}

doxygen_conf  = "doxygen.conf"

generate_pdf  = False
generate_zip  = True

def zip_dir(dirpath, outFullName):
    zip = zipfile.ZipFile(outFullName, "w", zipfile.ZIP_DEFLATED)
    for path, dirnames, filenames in os.walk(dirpath):
        fpath = path.replace(dirpath, '')

        for filename in filenames:
            zip.write(os.path.join(path, filename), os.path.join(fpath, filename))
    zip.close()

def build():
    if(len(args) <= 2):
        if(cmd == "-p"):
            if(args[1] == "release"):
                version = get_version()
                traversal(version)
            elif(args[1] == version_specific):
                traversal(version_specific)
            elif(args[1] == "--help" or args[1] == "-h"):
                print("\r")
                print("By default, the latest version is compiled\r")
                print("\r")
                print("Optional arguments:\r")
                print("               release        :    Compile the release version\r")
                print("               specific       :    Only compile the w800 latest version use zh_CN\r")
                print("               --help  , -h   :    Show this help message and exit\r")
                print("\r")
            else:
                print("Command error , Please enter - h for help information\r")
        elif(cmd == "-d"):
            traversal(version_default)
    else:
        print("Command error , Please enter - h for help information\r")

def traversal(version):
    socs = soc_dict.values()
    languages = language_dict.values()
    types = type_dict.values()

    doxygen_command()

    process_example_doc(version, True)

    if (version_specific != version):
        for soc in socs:
            for language in languages:
                for type in types:
                    if type == "latexpdf" and generate_pdf == False:
                        continue
                    combination_command(soc, language, version, type)
                file_operations(soc, language, version)
    else:
        combination_command("w800", "zh_CN", version_default, "html")
        file_operations("w800", "zh_CN", version)

    process_example_doc(version, False)

def get_version():
    abspath = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    with open(os.path.join(abspath, "version"), "r") as file:
        while True:
            file_content = file.readline()
            if not file_content:
                break
            if  "config BUILD_VERSION" in file_content:
                file_content = file.readline()
                file_content = file.readline()
                if "default" in file_content:
                    version = file_content.split("\"")[1]
                break
    return version

def find_main_directories(prj_path):
    for root, dirs, files in os.walk(prj_path, topdown=True):
        if 'main' in dirs and 'CMakeLists.txt' in files:
            main_dir = os.path.join(root, 'main')
            yield os.path.dirname(main_dir)

def has_image_files(folder_path, extensions):
    for ext in extensions:
        files = glob.glob(os.path.join(folder_path, '**', '*' + ext), recursive = True)
        if files:
            return True
    return False

def check_and_copy_images(main_dir, dst_dir, imgdirs = ('assert', 'figure', '_static'), extensions = ('.jpg', '.jpeg', '.png', '.gif', '.bmp', '.svg')):
    for imgdir in imgdirs:
        if has_image_files(main_dir + "/" + imgdir, extensions):
            shutil.copytree(main_dir + "/" + imgdir, dst_dir + "/" + imgdir)
    for ext in extensions:
        imgs = glob.glob(os.path.join(main_dir, '*' + ext), recursive = True)
        if imgs:
            for img in imgs:
                shutil.copy(img, dst_dir)

def process_example_doc_by_type(version, main_dir, abspath, prjdocc, prjdoce, cn_doc, en_doc):
    exist_doc = False
    cn_rdme = []
    en_rdme = []
    rdme = os.path.join(main_dir, cn_doc)
    if os.path.exists(rdme):
        #print("found " + rdme)
        os.makedirs(prjdocc + main_dir[len(abspath):], exist_ok = True)
        shutil.copy(rdme, prjdocc + main_dir[len(abspath):])
        check_and_copy_images(main_dir, prjdocc + main_dir[len(abspath):])
        cn_rdme.append(main_dir[len(abspath + "/"):] + "/" + os.path.basename(rdme).rsplit('.', 1)[0])
        exist_doc = True
    if (version_specific != version):
        rdme = os.path.join(main_dir, en_doc)
        if os.path.exists(rdme):
            #print("found " + rdme)
            os.makedirs(prjdoce + main_dir[len(abspath):], exist_ok = True)
            shutil.copy(rdme, prjdoce + main_dir[len(abspath):])
            check_and_copy_images(main_dir, prjdoce + main_dir[len(abspath):])
            en_rdme.append(main_dir[len(abspath + "/"):] + "/" + os.path.basename(rdme).rsplit('.', 1)[0])
            exist_doc = True
    return exist_doc,cn_rdme,en_rdme

def get_path_name_list(file_path):
    dir_path = os.path.dirname(file_path)
    dirs = []
    while True:
        dir_path, tail = os.path.split(dir_path)
        if tail:
            dirs.append(tail)
        else:
            break
    dirs.reverse()
    return dirs

def generate_example_index(is_cn, prjdoc, dirs, curr_index):
    file_name = ""
    for i in range(0, curr_index + 1):
        file_name = file_name + dirs[i] + "/"
    file_path = file_name + "index.rst"
    file_path = prjdoc + "/" + file_path
    if not os.path.exists(file_path):
        with open(file_path, "w", encoding="utf-8") as file:
            file_content = ".. _" + dirs[curr_index] + "_example:\n\n"
            file_content = file_content + dirs[curr_index] + "\n"
            file_content = file_content + "===================\n"
            file_content = file_content + "\n"
            file_content = file_content + ".. toctree::\n"
            file_content = file_content + "   :maxdepth: 1\n"
            file_content = file_content + "\n"
            file.write(file_content)
    if (len(dirs) - 1 - 1) == curr_index:
        with open(file_path, 'r', encoding='utf-8') as file:
            if is_cn:
                file_line = "   " + dirs[curr_index + 1] + " <" + dirs[curr_index + 1] + "/README" + ">\n"
            else:
                file_line = "   " + dirs[curr_index + 1] + " <" + dirs[curr_index + 1] + "/README_EN" + ">\n"
            file_content = file.read()
            if file_line not in file_content:
                with open(file_path, 'a', encoding='utf-8') as file:
                    file.write(file_line)
    else:
        with open(file_path, 'r', encoding='utf-8') as file:
            file_line = "   " + dirs[curr_index + 1] + " <" + dirs[curr_index + 1] + "/index" + ">\n"
            file_content = file.read()
            if file_line not in file_content:
                with open(file_path, 'a', encoding='utf-8') as file:
                    file.write(file_line)
        generate_example_index(is_cn, prjdoc, dirs, curr_index + 1)
def process_example_doc(version, flag):
    sys.stdout.flush()
    abspath = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    prjdocc = abspath + "/docs/zh_CN/examples_reference"
    prjdoce = abspath + "/docs/en/examples_reference"
    try:
        if os.path.exists(prjdocc + "/examples"):
            shutil.rmtree(r'' + prjdocc + "/examples")
        if (version_specific != version):
            if os.path.exists(prjdoce + "/examples"):
                shutil.rmtree(r'' + prjdoce + "/examples")
    except FileNotFoundError as e:#Exception
        pass
    if flag:
        sys.stdout.write("export example readme...")
        prjpath = abspath + "/examples"
        cn_rdme = []
        en_rdme = []
        for main_dir in find_main_directories(prjpath):
            exist_md,cn_doc,en_doc = process_example_doc_by_type(version, main_dir, abspath, prjdocc, prjdoce, "README.md", "README_EN.md")
            if False == exist_md:
                exist_md,cn_doc,en_doc = process_example_doc_by_type(version, main_dir, abspath, prjdocc, prjdoce, "README.rst", "README_EN.rst")
            if False != exist_md:
                if (len(cn_doc)):
                    cn_rdme.extend(cn_doc)
                if (len(en_doc)):
                    en_rdme.extend(en_doc)
        with open(prjdocc + "/index.rst", "w", encoding="utf-8") as file:
            file_content = "示例 参考\n"
            file_content = file_content + "===================\n"
            file_content = file_content + "\n"
            file_content = file_content + ".. toctree::\n"
            file_content = file_content + "   :maxdepth: 1\n"
            file_content = file_content + "\n"
            for rd in cn_rdme:
                dirs = get_path_name_list(rd)
                if 2 == len(dirs):
                    file_content = file_content + "   " + dirs[1] + " <" + rd.replace("\\", "/") + ">\n"
                else:
                    file_line = "   " + dirs[1] + " <" + dirs[0] + "/" + dirs[1] + "/index" + ">\n"
                    if file_line not in file_content:
                        file_content = file_content + file_line
                    generate_example_index(True, prjdocc, dirs, 1)
            file.write(file_content)
        if (version_specific != version):
            with open(prjdoce + "/index.rst", "w", encoding="utf-8") as file:
                file_content = "Examples Reference\n"
                file_content = file_content + "===================\n"
                file_content = file_content + "\n"
                file_content = file_content + ".. toctree::\n"
                file_content = file_content + "   :maxdepth: 1\n"
                file_content = file_content + "\n"
                for rd in en_rdme:
                    dirs = get_path_name_list(rd)
                    if 2 == len(dirs):
                        file_content = file_content + "   " + dirs[1] + " <" + rd.replace("\\", "/") + ">\n"
                    else:
                        file_line = "   " + dirs[1] + " <" + dirs[0] + "/" + dirs[1] + "/index" + ">\n"
                        if file_line not in file_content:
                            file_content = file_content + file_line
                        generate_example_index(False, prjdoce, dirs, 1)
                file.write(file_content)
    else:
        sys.stdout.write("clean example readme...")
        with open(prjdocc + "/index.rst", "w", encoding="utf-8") as file:
            file.write("# 该文件自动生成，请不要手动编辑")
        if (version_specific != version):
            with open(prjdoce + "/index.rst", "w", encoding="utf-8") as file:
                file.write("# Auto Generated, DO NOT edit this file")
    sys.stdout.write(" done\n")

def file_operations(soc, language, version):
    if (version_specific != version):
        route = os.path.join("build", soc, language, version)

        route_html = os.path.join(route, "html")
        filelist = os.listdir(route_html)

        for file in filelist:
            src = os.path.join(route_html, file)
            dst = os.path.join(route, file)
            shutil.move(src, dst)

        if generate_pdf:
            route_latexpdf = os.path.join(route, "latex", "wm-iot-sdk-docs.pdf")
            shutil.move(route_latexpdf, route)
        shutil.rmtree(r'' + route_html)
        if generate_pdf:
            shutil.rmtree(r'' + os.path.join(route,"latex"))
        shutil.rmtree(r'' + os.path.join(route,"doctrees"))
        os.unlink(r''+ os.path.join(route,"objects.inv"))
        os.unlink(r''+ os.path.join(route,".buildinfo"))

        if generate_zip and (version_specific != version):
            print("compressing the document...")
            route_zip = os.path.join("build", "wm-iot-sdk-docs.zip")
            zip_dir(route, route_zip)
            route_zip2 = os.path.join(route, "wm-iot-sdk-docs.zip")
            if os.path.exists(route_zip2):
                os.remove(route_zip2)
            shutil.move(route_zip, route)
            print("document compression complete")

def combination_command(soc, language, version, type):
    if "Windows" == platform.system():
        python_exec = "python"
    else:
        python_exec = sys.executable
    cmd_abspath = os.path.dirname(os.path.abspath(__file__))
    cmd = python_exec + " -m sphinx -M " + type + " " + cmd_abspath
    cmd = os.path.join(cmd, language +" ")

    cmd = cmd + os.path.join(cmd, " ", cmd_abspath, "build", soc,  language, version)

    cmd = cmd + " -c . " + "-t " +  soc + " -t " + version

    if (language == "zh_CN"):
        cmd = cmd + " -t lzh"
    elif(language == "en"):
        cmd = cmd + " -t len"

    if (version != version_default):
        cmd = cmd + " -t release"

    if type == "latexpdf":
        cmd = cmd + " -t pdf"

    os.system(cmd)

def doxygen_command():
    sys.stdout.write("process doxygen...")
    sys.stdout.flush()
    cmd_abspath = os.path.dirname(os.path.abspath(__file__))
    cmd = "doxygen " + cmd_abspath
    cmd = os.path.join(cmd, doxygen_conf)
    os.system(cmd)
    sys.stdout.write(" done\n")

if __name__ == '__main__':
    build()
