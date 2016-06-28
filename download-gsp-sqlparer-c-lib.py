# coding=utf-8

from distutils.core import setup, Extension
import sys
import os

ARCH='64'
SQLPARSER_DIR = './general_sql_parser/'

if sys.maxsize > 2**32:
    ARCH = '64'
else:
    ARCH = '32'

def download_library():
    import urllib

    file_name = "gsp_c_lib.tar.gz"

    url = "http://www.sqlparser.com/dl/gsqlparser_c_linux32_trial_0_3_8.tar.gz"
    if os.name == "nt":
        if ARCH == '32':
            url = "http://www.sqlparser.com/dl/gsqlparser_c_win32_trial_0_3_8.zip"
        else:
            url = "http://www.sqlparser.com/dl/gsqlparser_c_win64_trial_0_3_8.zip"
        file_name = "gsp_c_lib.zip"
    else:
        if ARCH == '64':
            url = "http://www.sqlparser.com/dl/gsqlparser_c_linux64_trial_0_3_8.tar.gz"

    print "Downloading library from '%s'..." % url

    urllib.urlretrieve(url, file_name)

    print "Done!"
    print "Extracting archive..."

    if os.name == "nt":
        import zipfile

        archive = zipfile.ZipFile(file_name, 'r')
        archive.extractall(SQLPARSER_DIR)
    else:
        import tarfile

        archive = tarfile.open(file_name)
        archive.extractall(SQLPARSER_DIR)

    print "Done!"


if __name__ == '__main__':
    if not os.path.isdir(SQLPARSER_DIR):
        print "Could not find the general sql parser library in %s" % SQLPARSER_DIR
        answer = raw_input("Do you want to download it now? (Y, N): ")

        if answer and answer.upper() == 'Y':
            download_library()
    else:
        print SQLPARSER_DIR ,'exits, do nothing.'

