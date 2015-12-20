#pragma once

namespace px {
	class Response;
	typedef std::shared_ptr<Response> Response_p;
	
	class Response {
	public:
	private:
		std::string m_data;
		short m_status_code;
	};
}