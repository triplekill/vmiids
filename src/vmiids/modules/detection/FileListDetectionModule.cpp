/*
 * FileListDetectionModule.cpp
 *
 *  Created on: Jul 28, 2010
 *      Author: kittel
 */

#include "FileListDetectionModule.h"

LOADMODULE(FileListDetectionModule);


FileListDetectionModule::FileListDetectionModule() :
					DetectionModule("FileListDetectionModule") {
	GETSENSORMODULE(this->qemu, QemuMonitorSensorModule);
	GETSENSORMODULE(this->shell, ShellSensorModule);
	GETSENSORMODULE(this->fs, FileSystemSensorModule);

	GETOPTION(directory, this->directory);

	while(directory.size() > 1 && directory[directory.size()-1] == '/')
		directory.erase(directory.size()-1);

}

FileListDetectionModule::~FileListDetectionModule() {

}

void FileListDetectionModule::run() {

	bool isRunning;
	try {
		isRunning = this->qemu->isRunning();
	} catch (vmi::ModuleException &e) {
		critical << "Could not use QemuMonitorSensorModule";
		return;
	}

	//Get filelist from fs
	std::set<std::string> fsFileList;
	this->fs->getFileList(this->directory, fsFileList);

	if (!isRunning) {
		this->qemu->resumeVM();
	}

	//Get filelist from ls
	std::set<std::string> shellFileList;
	this->shell->getFileList(this->directory, shellFileList);

	if (!isRunning) {
		this->qemu->pauseVM();
	}

	float intrusion = 0;

	//Compare Process Lists
	std::set<std::string>::iterator s_it;
	std::set<std::string>::iterator f_it;

	//Find files in find not listed in FileSystem
	for (s_it = shellFileList.begin(); s_it != shellFileList.end(); s_it++) {
		if (fsFileList.find(*s_it) == fsFileList.end()) {
			critical << "File not found on FileSystem: " << *s_it << std::endl;
			intrusion = 0.5;
		} else {
			fsFileList.erase(fsFileList.find(*s_it));
			shellFileList.erase(s_it);
		}
	}

	for (f_it = fsFileList.begin(); f_it != fsFileList.end(); f_it++){
		critical << "File not found with find command: " << *f_it << std::endl;
		intrusion = 0.5;
	}

	//Compare with results of last run
	//FileSystem results
	for (f_it = fsFileList.begin(); f_it != fsFileList.end(); f_it++) {
		if (this->globalFsFileList.find(*f_it) != this->globalFsFileList.end()) {
			alert << "Hidden file detected: " << (*f_it) << std::endl;
			intrusion = 1;
		}
	}
	this->globalFsFileList = fsFileList;

	//find results
	for (s_it = shellFileList.begin(); s_it != shellFileList.end(); s_it++) {
		if (this->globalShellFileList.find(*s_it) != this->globalShellFileList.end()) {
			alert << "Virtual file detected: " << (*s_it) << std::endl;
			intrusion = 1;
		}
	}
	this->globalShellFileList = shellFileList;
	this->threatLevel = intrusion;
}
