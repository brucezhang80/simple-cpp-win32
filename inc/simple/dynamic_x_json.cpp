#include <cassert>
#include <string>
#include <stack>
#include <iostream>

#include "simple/json.h"
#include "simple/string.h"
#include "simple/switch.h"
#include "dynamic.h"

bool DynamicValue_to_json(std::ostream& os, const DynamicValue& value) {
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

struct	DynamicData_json_serializer {
    void	item_iterator(const DynamicData::traverse_event& item) {
        bool	TI	= DynamicData::TRAVERSE_ITEM		== item.type;
        bool	TA	= DynamicData::TRAVERSE_ARRAY_ITEM	== item.type;
        bool	TO	= DynamicData::TRAVERSE_OBJECT_ITEM	== item.type;
        bool	TG	= DynamicData::TRAVERSE_GROUP_END	== item.type;
        bool	VA	= DynamicData::VALUE_ARRAY			== item.item.type();
        bool	VO	= DynamicData::VALUE_OBJECT			== item.item.type();

        switch FLAG6(TI, TA, TO, TG, VA, VO) {
        case COND6(T,F,F,F,F,F): {
            DynamicValue_to_json(ss_,item.item);
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
            DynamicValue_to_json(ss_,item.item);
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
            DynamicValue_to_json(ss_, item.item);
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

    DynamicData_json_serializer(std::ostream& ss):ss_(ss) {}
private:
    std::ostream&		ss_;
};

bool DynamicData_to_json(std::ostream& os, DynamicData& data) {
    DynamicData_json_serializer	it(os);
    data.traverse(bind(&DynamicData_json_serializer::item_iterator, &it));
    return	os.good();
}
/*
void	JSON_DeSerializeNode(json_t* node, DynamicValue& data)
{
	switch(node->type)
	{
	case JSON_STRING:	data.SetValue(std::string(node->text));			break;
	case JSON_NUMBER:	data.SetValue(atof(node->text));	break;
	case JSON_OBJECT:
		{
			data.ClearAsObject();
			for(json_t* it = node->child; it != NULL; it = it->next)
			{
				JSON_DeSerializeNode(it->child, data[it->text]);
			}
		}break;
	case JSON_ARRAY:
		{
			data.ClearAsArray();
			json_t* it	= node->child;
			for(int	i = 0; it != NULL; it = it->next, ++i)
			{
				JSON_DeSerializeNode(it, data[i]);
			}
		}break;
	case JSON_TRUE:		data.SetValue(true);	break;
	case JSON_FALSE:	data.SetValue(false);	break;
	case JSON_NULL:		data.Clear();			break;
	default:
		{
			assert(false);
			data.Clear();
		}
	}
}

bool	JSON_DeSerializeData(const char* pszJSON,  DynamicData& data)
{
	json_t*	root = NULL;
	if(JSON_OK != json_parse_document(&root, pszJSON))
	{
		return	false;
	}

	JSON_DeSerializeNode(root, data);

	json_free_value(&root);
	return	true;
}
*/