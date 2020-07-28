#!/usr/bin/python
#
# BSD-3 Clause.
# Copyright (C) 2018 Antony Jr.
#
# Simple Cross Platform Installer Script
import sys
import os
import requests
from shutil import rmtree

# Packages to install
QEasyDownloader = {
        "username" : "antony-jr",
        "repo"     : "QEasyDownloader",
        "mkdir"    : {
            "QEasyDownloader/src",
            "QEasyDownloader/include"
        },
        "install"  : {
             "CMakeLists.txt"       : "QEasyDownloader/CMakeLists.txt",
             "QEasyDownloader.pro"         : "QEasyDownloader/QEasyDownloader.pro",
             "QEasyDownloader"             : "QEasyDownloader/QEasyDownloader",
             "include/QEasyDownloader.hpp" : "QEasyDownloader/include/QEasyDownloader.hpp",
             "src/QEasyDownloader.cc"      : "QEasyDownloader/src/QEasyDownloader.cc",
             "LICENSE"      : "QEasyDownloader/LICENSE"
         }
}

def installPackage(config):
    print("Installing " + config["repo"])
    # Make parent directory first.
    if os.path.isfile(config["repo"]):
        print("Deleting duplicate file(s)... ")
        os.remove(config["repo"])
        os.mkdir(config["repo"])
    else:
     if os.path.exists(config["repo"]):
         rmtree(config["repo"])
     else:
         os.mkdir(config["repo"])

    for i in config["mkdir"]:
     print("Creating Directory " + i)
     if os.path.exists(i):
         rmtree(i)
     os.mkdir(i) # Make the directory!
    print("Downloading the latest release from github... ")

    # Write files from the repo
    for i in config["install"]:
        resp = requests.get("https://raw.githubusercontent.com/"+config["username"]+"/"+config["repo"]+"/master/" + i)
        fp = open(config["install"][i] , "wb")
        for it in resp:
            fp.write(it)
        fp.close()

    print("Installed "+config["repo"]+".")
    return True

if __name__ == "__main__":
    installPackage(QEasyDownloader)
    sys.exit(0)
