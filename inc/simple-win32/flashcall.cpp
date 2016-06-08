#include <string>
#include <stack>
#include <iostream>

#include "simple/string.h"
#include "simple/switch.h"
#include "simple/rapidxml.h"
#include "simple-win32/dynamic.h"

bool FlashCallXML_SerializeValue(std::ostream& os, const DynamicValue& value) {
    switch (value.type()) {
    case DynamicValue::VALUE_STRING: {
        std::string stringVal = value.as_string();
        string_replace(stringVal, "&",	"&amp;");
        string_replace(stringVal, "\"",	"&quot;");
        string_replace(stringVal, "'",	"&apos;");
        string_replace(stringVal, "<",	"&lt;");
        string_replace(stringVal, ">",	"&gt;");

        return	(os	<< "<string>" << stringVal << "</string>").good();
    }
    break;
    case DynamicValue::VALUE_ARRAY:
    case DynamicValue::VALUE_OBJECT: {
        assert(false);
        return	false;
    }
    break;
    case DynamicValue::VALUE_BOOL: {
        return	(os	<< (value.as_bool() ? "<true/>" : "<false/>")).good();
    }
    break;
    case DynamicValue::VALUE_EMPTY: {
        return	(os	<< "<null/>").good();
    }
    break;
    default: { // number
        return	(os	<< "<number>" << value.as_string() << "</number>").good();
    }
    break;
    }

    return (os << "<null/>").good();
}

struct	FlashCallXML_DynamicDataSerializer {
    void	ItemIterator(const DynamicData::TraverseEvent& item) {
        bool	TI	= DynamicData::TRAVERSE_ITEM		== item.type;
        bool	TA	= DynamicData::TRAVERSE_ARRAY_ITEM	== item.type;
        bool	TO	= DynamicData::TRAVERSE_OBJECT_ITEM	== item.type;
        bool	TG	= DynamicData::TRAVERSE_GROUP_END	== item.type;
        bool	VA	= DynamicData::VALUE_ARRAY			== item.item.type();
        bool	VO	= DynamicData::VALUE_OBJECT			== item.item.type();

        switch( FLAG6(TI, TA, TO, TG, VA, VO) ) {
        case COND6(T,F,F,F,F,F): {
            FlashCallXML_SerializeValue(ss_,item.item);
        }
        break;
        case COND6(T,F,F,F,T,F): {
            ss_<<"<array>";
            pca_.push(false);
        }
        break;
        case COND6(T,F,F,F,F,T): {
            ss_<<"<object>";
            pco_.push(false);
        }
        break;

        case COND6(F,T,F,F,F,F): {
            ss_<<"<property id=\""<<item.index<< "\">";
            FlashCallXML_SerializeValue(ss_,item.item);
            ss_<<"</property>";
        }
        break;
        case COND6(F,T,F,F,T,F): {
            ss_<<"<property id=\""<<item.index<< "\">";
            ss_<<"<array>";
            pca_.push(true);
        }
        break;
        case COND6(F,T,F,F,F,T): {
            ss_<<"<property id=\""<<item.index<< "\">";
            ss_<<"<object>";
            pco_.push(true);
        }
        break;

        case COND6(F,F,T,F,F,F): {
            ss_<<"<property id=\""<<item.key<< "\">";
            FlashCallXML_SerializeValue(ss_, item.item);
            ss_<<"</property>";
        }
        break;
        case COND6(F,F,T,F,T,F): {
            ss_<<"<property id=\""<<item.key<< "\">";
            ss_<<"<array>";
            pca_.push(true);
        }
        break;
        case COND6(F,F,T,F,F,T): {
            ss_<<"<property id=\""<<item.key<< "\">";
            ss_<<"<object>";
            pco_.push(true);
        }
        break;

        case COND6(F,F,F,T,T,F): {
            if(!pca_.empty()) {
                ss_<<"</array>";
                if(pca_.top())ss_<<"</property>";
                pca_.pop();
            }
        }
        break;
        case COND6(F,F,F,T,F,T): {
            if(!pco_.empty()) {
                ss_<<"</object>";
                if(pco_.top())ss_<<"</property>";
                pco_.pop();
            }
        }
        break;
        }
    }

    FlashCallXML_DynamicDataSerializer(std::ostream& ss):ss_(ss) {}
private:
    std::ostream&		ss_;
    std::stack<bool>	pca_;
    std::stack<bool>	pco_;
};

bool FlashCallXML_SerializeData(std::ostream& os, DynamicData& data) {
    FlashCallXML_DynamicDataSerializer	it(os);
    data.traverse(bind(&FlashCallXML_DynamicDataSerializer::ItemIterator, &it));
    return	os.good();
}

bool FlashCallXML_SerializeInvoke(std::ostream& os,
                                  const std::string&				func_name,
                                  const std::deque<DynamicData*>& args
                                 ) {
    os	<< "<invoke name=\"" << func_name << "\" returntype=\"xml\">";
    if(!args.empty()) {
        os << "<arguments>";
        std::deque<DynamicData*>::const_iterator	it		= args.begin();
        std::deque<DynamicData*>::const_iterator	it_end	= args.end();
        for(; it != it_end; ++it) {
            assert(NULL != *it);
            FlashCallXML_SerializeData(os, *(*it));
        }
        os << "</arguments>";
    }
    os << "</invoke>";

    return os.good();
}


bool FlashCallXML_SerializeInvoke(std::ostream& os,
                                  const std::string&				func_name,
                                  DynamicData*					args,
                                  size_t							args_sum
                                 ) {
    std::deque<DynamicData*>	real_args;
    for(size_t i = 0 ; i < args_sum; ++i) {
        real_args.push_back(args + i);
    }

    return	FlashCallXML_SerializeInvoke(os, func_name, real_args);
}

void	FlashCallXML_DeSerializeNode(rapidxml::xml_node<>* parent_node, DynamicValue& data) {
    using	namespace rapidxml;
    std::string	sNodeType(parent_node->name(), parent_node->name_size());
    if("string" == sNodeType) {
        data.set_value(std::string(parent_node->value(), parent_node->value_size()));
    } else if("number" == sNodeType) {
        double	d	= atof(parent_node->value());
        data.set_value(d);
    } else if("true" == sNodeType) {
        data.set_value(true);
    } else if("false" == sNodeType) {
        data.set_value(false);
    } else if("object" == sNodeType) {
        data.clear_as_object();
        for(xml_node<> *node = parent_node->first_node("property");
                NULL != node;
                node	= node->next_sibling("property")) {
            xml_attribute<> *attr		= node->first_attribute("id");
            xml_node<>		*data_node	= node->first_node();
            FlashCallXML_DeSerializeNode(data_node, data[attr->value()]);
        }
    } else if("array" == sNodeType) {
        data.clear_as_array();
        for(xml_node<> *node = parent_node->first_node("property");
                NULL != node;
                node	= node->next_sibling("property")) {
            xml_attribute<> *attr		= node->first_attribute("id");
            xml_node<>		*data_node	= node->first_node();
            FlashCallXML_DeSerializeNode(data_node, data[atoi(attr->value())]);
        }
    } else {
        assert("null" == sNodeType || "undefined" == sNodeType);
        data.clear();
    }
}


bool	FlashCallXML_DeSerializeData(const char* pszXML,  DynamicData& data) {
    using	namespace rapidxml;

    xml_document<> doc;			// character type defaults to char
    doc.parse<0>(const_cast<char*>(pszXML));		// 0 means default parse flags

    FlashCallXML_DeSerializeNode(doc.first_node(), data);

    return	true;
}

bool	FlashCallXML_DeSerializeInvoke(const char* pszXML,
                                       std::string&				func_name,
                                       std::deque<DynamicData*>&	arguments
                                   ) {
    using	namespace rapidxml;

    xml_document<> doc;			// character type defaults to char
    doc.parse<0>(const_cast<char*>(pszXML));		// 0 means default parse flags
    xml_node<> *invoke		= doc.first_node("invoke");
    if(NULL == invoke) {
        return	false;
    }
    xml_attribute<>* func	= invoke->first_attribute("name");
    if(NULL == func) {
        return	false;
    }
    func_name.assign(func->value(), func->value_size());

    arguments.clear();
    xml_node<> *args		= invoke->first_node("arguments");
    if(NULL != args) {
        for(xml_node<> *node	= args->first_node();
                NULL != node;
                node = node->next_sibling()) {
            arguments.push_back(new DynamicData());
            FlashCallXML_DeSerializeNode(node, *(arguments.back()));
        }
    }

    return	true;
}

void	FlashCallXML_ClearArguments	(std::deque<DynamicData*>& arguments) {
    std::deque<DynamicData*>::iterator	it		= arguments.begin();
    std::deque<DynamicData*>::iterator	it_end	= arguments.end();
    for(; it != it_end; ++it) {
        delete	*it;
    }
    arguments.clear();
}

