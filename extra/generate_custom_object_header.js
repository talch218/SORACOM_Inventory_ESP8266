const fs = require('fs');
const path = require('path');

const getMemberName = resource => `${resource.name.replace(' ', '')}`;

const generate_header = json => `\
#pragma once

#include <lwm2m/objects.h>
namespace id${json.id} {
${Object.entries(json.resources)
	.filter(([k,v]) => v.type == 'STRING')
	.map(([k,v]) => `\
#ifndef ${getMemberName(v)}${json.id}
class ${getMemberName(v)}Type : public PreallocString<30> {};
#endif

`).join('')}
class instance : public Lwm2mObjectInstance {
public:
${Object.entries(json.resources)
	.map(([k,v]) => `\
	${v.operations === 'E' ? 'Executable' : ({
		STRING: `${getMemberName(v)}Type`,
		BOOLEAN: 'bool',
		INTEGER: 'int32_t',
		FLOAT: 'double'
	})[v.type]} ${getMemberName(v)};
`).join('')}};

enum class ResourceId {
${Object.entries(json.resources)
	.map(([k,v]) => `\
	${getMemberName(v)} = ${v.id},
`).join('')}};

class object : public Lwm2mObject<${json.id}, object, instance> {
public:
${Object.entries(json.resources)
	.map(([k,v]) => `\
	Resource(${v.id}, &instance::${getMemberName(v)}, O_RES_${v.operations}) ${getMemberName(v)};
`).join('')}};
} // End of namespace
inline bool operator== (::id${json.id}::ResourceId c1, uint16_t c2) { return (uint16_t) c1 == c2; }
inline bool operator== (uint16_t c2, id${json.id}::ResourceId c1) { return (uint16_t) c1 == c2; }
`;

process.argv.slice(2).forEach(async file => {
	const json = await new Promise((resolve, reject) => {
		fs.readFile(file, (err, data) => {
			if (err) reject(err);
			else resolve(JSON.parse(data));
		});
	});

	fs.writeFileSync(
		path.join(path.dirname(file), path.basename(file, '.json') + '.h'),
		generate_header(json)
	);
});
