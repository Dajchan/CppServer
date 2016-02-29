//#pragma once
//#include "defines.hpp"
///*
//	{EACH pictures p}
//	<img src="{SCOPE p image_url-}" />
//	{-EACH pictures}
//*/
//
//namespace px {
//	
//	typedef enum {
//		NONE=0,
//		SCOPE=1,
//		EACH=2,
//	} ViewTemplateCommand;
//	
//	struct ViewTemplatePart {
//		string content;
//		ViewTemplateCommand cmd;
//		vector<string> params;
//		vector<ViewTemplatePart> *subparts;
//	};
//	
//	class ViewTemplate;
//	typedef shared_ptr<ViewTemplate> ViewTemplate_p;
//	
//	class ViewTemplate {
//	public:
//		ViewTemplate_p New(const string& filePath);
//		~ViewTemplate();
//		
//		void each_part(const function<void(const ViewTemplatePart& part)>& lambda);
//		
//	private:
//		vector<ViewTemplatePart> * m_parts;
//		
//		ViewTemplate(vector<ViewTemplatePart> *);
//		
//		ViewTemplate() = delete;
//		ViewTemplate(const ViewTemplate&) = delete;
//	};
//}
