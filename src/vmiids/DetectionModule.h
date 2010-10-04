/*
 * DetectionModule.h
 *
 *  Created on: Jun 26, 2010
 *      Author: kittel
 */

#ifndef DETECTIONMODULE_H_
#define DETECTIONMODULE_H_

#include "Module.h"
#include "OutputModule.h"
#include "vmiids/util/Thread.h"

#include "vmiids/util/Mutex.h"

#include <string>
#include <map>
#include <list>

namespace vmi {

class DetectionModule : public vmi::Module, protected OutputModule, public Thread{
	private:
		static std::map<std::string, vmi::DetectionModule *> modules;
		static vmi::Mutex mutex;

	protected:
		float intrusionDetected;

	public:
		DetectionModule(std::string moduleName);
		virtual ~DetectionModule();

		static vmi::DetectionModule *getDetectionModule(std::string detectionModuleName);

		static std::list<std::string> getListOfDetectionModules();

		static void killInstances();

		float getThreadLevel();
};

}

#endif /* DETECTIONMODULE_H_ */

