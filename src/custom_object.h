#pragma once

#include <lwm2m/objects.h>
namespace id40001 {
#ifndef Message40001
class MessageType : public PreallocString<30> {};
#endif

#ifndef Update40001
class UpdateType : public PreallocString<30> {};
#endif


class instance : public Lwm2mObjectInstance {
public:
	MessageType Message;
	bool LED;
	int32_t AnalogOutput;
	double SensorValue;
	Executable Update;
};

enum class ResourceId {
	Message = 1,
	LED = 2,
	AnalogOutput = 3,
	SensorValue = 4,
	Update = 5,
};

class object : public Lwm2mObject<40001, object, instance> {
public:
	Resource(1, &instance::Message, O_RES_W) Message;
	Resource(2, &instance::LED, O_RES_RW) LED;
	Resource(3, &instance::AnalogOutput, O_RES_W) AnalogOutput;
	Resource(4, &instance::SensorValue, O_RES_R) SensorValue;
	Resource(5, &instance::Update, O_RES_E) Update;
};
} // End of namespace
inline bool operator== (::id40001::ResourceId c1, uint16_t c2) { return (uint16_t) c1 == c2; }
inline bool operator== (uint16_t c2, id40001::ResourceId c1) { return (uint16_t) c1 == c2; }
