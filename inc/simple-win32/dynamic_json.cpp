#include <cassert>
#include <string>
#include <stack>
#include <iostream>

#include "simple/json.h"
#include "simple/string.h"
#include "simple/switch.h"
#include "dynamic.h"

bool DynamicValue_ToJSON(std::ostream& os, const DynamicValue& value) {
    switch (value.type()) {
    case DynamicValue::VALUE_STRING: {
        return	(os	<< "\"" << string_json_escape(value.as_string().operator const char *()) << "\"").good();
    }
    break;
    case DynamicValue::VALUE_ARRAY:
    case DynamicValue::VALUE_OBJECT: {
        assert(false);
        return	false;
    }
    break;
    case DynamicValue::VALUE_BOOL: {
        return	(os	<< (value.as_bool() ? "true" : "false")).good();
    }
    break;
    case DynamicValue::VALUE_EMPTY: {
        return	(os	<< "null").good();
    }
    break;
    default: { // number
        return	(os	<< value.as_string()).good();
    }
    break;
    }

    return (os << "null").good();
}

struct	DynamicData_JSONSerializer {
    void	ItemIterator(const DynamicData::TraverseEvent& item) {
        bool	TI	= DynamicData::TRAVERSE_ITEM		== item.type;
        bool	TA	= DynamicData::TRAVERSE_ARRAY_ITEM	== item.type;
        bool	TO	= DynamicData::TRAVERSE_OBJECT_ITEM	== item.type;
        bool	TG	= DynamicData::TRAVERSE_GROUP_END	== item.type;
        bool	VA	= DynamicData::VALUE_ARRAY			== item.item.type();
        bool	VO	= DynamicData::VALUE_OBJECT			== item.item.type();

        switch FLAG6(TI, TA, TO, TG, VA, VO) {
        case COND6(T,F,F,F,F,F): {
            DynamicValue_ToJSON(ss_,item.item);
            if(item.remain_siblings > 0)ss_<<',';
        }
        break;
        case COND6(T,F,F,F,T,F): {
            ss_<<'[';
        }
        break;
        case COND6(T,F,F,F,F,T): {
            ss_<<'{';
        }
        break;

        case COND6(F,T,F,F,F,F): {
            DynamicValue_ToJSON(ss_,item.item);
            if(item.remain_siblings > 0)ss_<<',';
        }
        break;
        case COND6(F,T,F,F,T,F): {
            ss_<<'[';
        }
        break;
        case COND6(F,T,F,F,F,T): {
            ss_<<'{';
        }
        break;

        case COND6(F,F,T,F,F,F): {
            ss_<<'\"'<<item.key<<'\"'<<':';
            DynamicValue_ToJSON(ss_, item.item);
            if(item.remain_siblings > 0)ss_<<',';
        }
        break;
        case COND6(F,F,T,F,T,F): {
            ss_<<'\"'<<item.key<<'\"'<<':'<<'[';
        }
        break;
        case COND6(F,F,T,F,F,T): {
            ss_<<'\"'<<item.key<<'\"'<<':'<<'{';
        }
        break;

        case COND6(F,F,F,T,T,F): {
            ss_<<']';
            if(item.remain_siblings > 0)ss_<<',';
        }
        break;
        case COND6(F,F,F,T,F,T): {
            ss_<<'}';
            if(item.remain_siblings > 0)ss_<<',';
        }
        break;
        }
    }

    DynamicData_JSONSerializer(std::ostream& ss):ss_(ss) {}
private:
    std::ostream&		ss_;
};

bool DynamicData_ToJSON(std::ostream& os, DynamicData& data) {
    DynamicData_JSONSerializer	it(os);
    data.traverse(bind(&DynamicData_JSONSerializer::ItemIterator, &it));
    return	os.good();
}

static	void	JSON_DeSerializeNode(const json::Value& node, DynamicValue& data) {
    using	namespace	json;
    switch(node.GetType()) {
    case json::StringVal: {
        data.set_value((const std::string&)(node));
    }
    break;
    case json::IntVal: {
        data.set_value(node.ToInt());
    }
    break;
    case json::FloatVal: {
        data.set_value(node.ToFloat());
    }
    break;
    case json::DoubleVal: {
        data.set_value(node.ToDouble());
    }
    break;
    case json::ObjectVal: {
        data.clear_as_object();
        json::Object	obj	= node.ToObject();
        json::Object::ValueMap::const_iterator	it = obj.begin(), it_end = obj.end();
        for(; it != it_end; ++it) {
            JSON_DeSerializeNode(it->second, data[it->first]);
        }
    }
    break;
    case json::ArrayVal: {
        data.clear_as_array();
        json::Array	arr	= node.ToArray();
        json::Array::ValueVector::const_iterator	it = arr.begin(), it_end = arr.end();
        for(int	i = 0; it != it_end; ++it, ++i) {
            JSON_DeSerializeNode(*it, data[i]);
        }
    }
    break;
    case json::BoolVal: {
        data.set_value(node.ToBool());
    }
    break;
    case json::NULLVal: {
        data.clear();
    }
    break;
    default: {
        assert(false);
        data.clear();
    }
    }
}

bool	DynamicData_FromJSON		(DynamicData& data,		const std::string& pszJSON) {
    json::Value	root	= json::Deserialize(pszJSON);

    JSON_DeSerializeNode(root, data);

    return	true;
}
