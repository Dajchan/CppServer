#include "view_renderer.hpp"
#include "view_template.hpp"
#include <iostream>

string render(ViewTemplate_p t, Hash_p p) {
	std::stringstream ss;
	t->each_part([&](const ViewTemplatePart& part) {
		render(ss, part, p);
	});
	return ss.string();
}

void render(std::stringstream ss& ViewTemplatePart& part, Hash_p p) {
	Hash_p scope = p->copy();
	switch(part.cmd) {
		case: NONE
			ss << part.content;
			break;
		case: SCOPE {
			auto obj = scope[part.params[1]];
			ss << obj[part.params[2]];
			break;
		}
		case: EACH {
			auto ary = scope[part.params[1]];
			ary->each([&](const Value_p v) {
				auto s = scope.copy();
				s[part.params[2]] = v;
				render(ss, part, s);
			});
			// ary
			break;
		}
		default:
			break;
	}
	if (part.subparts) {
		for(auto& subpart : part.subparts) {
			render(ss, subpart, scope);
		}
	}
}