#include "RBXClasses.h"
#include "Memory.h"

Instance DataModel::GetService(std::string className) {
	return this->FindFirstChildOfClass(className);
}