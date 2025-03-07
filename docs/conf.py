# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
import os
import sys
import platform
import datetime

sdk_root = os.path.abspath(os.path.join(__file__, '..', '..'))

sys.path.insert(0, sdk_root + "/docs/_extensions")

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

current_year = datetime.datetime.now().year
copyright = '2022 - {} '.format(current_year)

is_release = tags.has('release')
is_pdf = tags.has('pdf')

soc_type = 'w800'
#for sc in ["w800", "w801s", "w803", "w806"]:
#    if tags.has(sc):
#        soc_type = sc
#        break

if tags.has('len'):
    project = 'WM IoT SDK Programming Guide'
    copyright = copyright + 'Beijing WinnerMicroelectronics Co.,Ltd'
    language = 'en'
    html_title = 'WinnerMicro Online Documentation'
    hv_download_doc = 'Download as ZIP'
    hv_select_version = 'Select Version'
    hv_lang_desc = 'Language'
    hv_soc_desc = 'SoC Type'
    hv_doc_type = 'Document Version'
    hl_latest_desc = 'This is the documentation for the latest (main) development branch. If you are looking for the documentation of previous releases, use the drop-down menu on the left and select the desired version.'
    hl_release_left = 'The'
    hl_release_midl = 'latest development version'
    hl_release_midr = 'of this page may be more current than this released'
    hl_release_right = 'version.'
    hb_edit_doc = 'Modify'
    hb_language = 'zh_CN'
    hb_language_desc = '中文'
else:
    project = 'WM IoT SDK 编程指南'
    copyright = copyright + '北京联盛德微电子有限责任公司'
    language = 'zh_CN'
    html_title = 'WinnerMicro 在线文档'
    hv_download_doc = '下载为ZIP'
    hv_select_version = '选择版本'
    hv_lang_desc = '语言'
    hv_soc_desc = '芯片类型'
    hv_doc_type = '文档版本'
    hl_latest_desc = '这是最新(主)开发分支的文档。如果您正在查找以前版本的文档，使用左侧的下拉菜单选择所需的版本。'
    hl_release_left = '这个页面的'
    hl_release_midl = '最新开发版本'
    hl_release_midr = '可能比这个发布的'
    hl_release_right = '版本新。'
    hb_edit_doc = '修改文档'
    hb_language = 'en'
    hb_language_desc = 'English'

author = 'WinnerMicro'

with open(sdk_root + "/version", "r") as file:
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
if not version:
    sys.stderr.write("Warning: Could not prase version\n")
    version = "Unknown"

release = version

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "sphinx.ext.todo",
    "sphinx.ext.extlinks",
    "sphinx.ext.autodoc",
    "sphinx.ext.graphviz",
    'recommonmark',
    'sphinx_markdown_tables',
    'sphinxcontrib.mermaid',
    'vcs_link',
    'breathe'
]

templates_path = ['_templates']
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    'navigation_depth': 5,
}

html_static_path = ['_static']
html_css_files = ['wm_theme.css']
html_js_files = ['wm_script.js']
html_logo = sdk_root + "/docs/_static/wm_logo_html.png"
html_favicon = sdk_root + "/docs/_static/favicon.png"
html_split_index = True
html_show_sourcelink = False
html_show_sphinx = False
docs_title = soc_type + " / {}".format(version if is_release else "latest")
html_context = {
    "docs_title": docs_title,
    "is_release": is_release,
    "current_version": release,
    "current_soc": soc_type,
    "current_language": language,
    "hv_download_doc": hv_download_doc,
    "hv_select_version": hv_select_version,
    "hv_lang_desc": hv_lang_desc,
    "hv_soc_desc": hv_soc_desc,
    "hv_doc_type": hv_doc_type,
    "hl_latest_desc": hl_latest_desc,
    "hl_release_left": hl_release_left,
    "hl_release_midl": hl_release_midl,
    "hl_release_midr": hl_release_midr,
    "hl_release_right": hl_release_right,
    "hb_edit_doc": hb_edit_doc,
    "hb_language": hb_language,
    "hb_language_desc": hb_language_desc,
    "soc_type": (
        ("w800"),
#        ("w801s"),
#        ("w802"),
#        ("w803"),
#        ("w805"),
#        ("w806"),
    ),
    "versions": (
        ("latest", "/" + language + "/latest/"),
        ("2.0.0rc2", "/" + language + "/2.0.0rc2/"),
        ("2.1.0rc1", "/" + language + "/2.1.0rc1/"),
        ("2.2-beta.2", "/" + language + "/2.2-beta.2/"),
        ("2.3-beta.2", "/" + language + "/2.3-beta.2/"),
    ),
    "display_vcs_link": True
}

# latex Configuration
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-latex-output
latex_engine = "xelatex"
latex_elements = {
    'papersize': 'a4paper',
    'utf8extra': '',
    'inputenc': '',
    'cmappkg': '',
    'extraclassoptions': 'openany,oneside',
    'fontpkg': r'''
        \setmainfont{DejaVu Serif}
        \setsansfont{DejaVu Sans}
        \setmonofont{DejaVu Sans Mono}
        ''',
    'preamble': r'''
        \usepackage{hyperref}
        \usepackage{xeCJK}
        \parindent 2em
        \setcounter{tocdepth}{3}
        \renewcommand\familydefault{\ttdefault}
        \renewcommand\CJKfamilydefault{\CJKrmdefault}
        \usepackage[titles]{tocloft}
        \cftsetpnumwidth {1.25cm}\cftsetrmarg{1.5cm}
        \setlength{\cftchapnumwidth}{0.75cm}
        \setlength{\cftsecindent}{\cftchapnumwidth}
        \setlength{\cftsecnumwidth}{1.25cm}
    ''',
    'fncychap': r'\usepackage[Bjornstrup]{fncychap}',
    'printindex': r'\footnotesize\raggedright\printindex',
}
latex_logo = os.path.join(sdk_root, "docs", "_static", "wm_logo_large.png")
latex_documents = [
    ("index", " wm-iot-sdk-docs.tex", project,
    "WinnerMicro", 'manual'),
]
latex_show_urls = 'footnote'

# mermaid Configuration
if is_pdf:
    mermaid_output_format = "png"
    if "Windows" == platform.system():
        mermaid_cmd = "mmdc.cmd"
    else:
        mermaid_cmd = "mmdc"

# vcs_link Configuration
if is_release:
    vcs_link_version = "v" + version
else:
    vcs_link_version =  "master"
vcs_link_base_url = "https://github.com/winnermicro/wm_iot_sdk/blob/" + vcs_link_version
vcs_link_prefixes = {
    ".*": "docs/" + language,
}

# Breathe Configuration
# https://breathe.readthedocs.io/en/latest/directives.html
breathe_default_project = 'wm-iot-sdk-apis'
breathe_projects = {breathe_default_project: os.path.join(sdk_root, 'docs/build/xml')}
breathe_show_include = False
