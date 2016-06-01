#include <cassert>
#include "dynamic.h"

#pragma	comment(lib, "comsupp.lib")

void dynamic_value::clear_as_array() {
    internal_as_data().clear_as_array();
    value_.vt	= VALUE_ARRAY;
}

void dynamic_value::clear_as_object() {
    internal_as_data().clear_as_object();
    value_.vt	= VALUE_OBJECT;
}

dynamic_data* dynamic_value::as_data()const {
    switch(value_.vt) {
    case VALUE_ARRAY:
    case VALUE_OBJECT: {
        return reinterpret_cast<dynamic_data*>(value_.pintVal);
    }
    break;
    }

    return	NULL;
}

dynamic_data& dynamic_value::internal_as_data() {
    dynamic_data*	pArray	= NULL;
    switch(value_.vt) {
    case VALUE_ARRAY:
    case VALUE_OBJECT: {
        pArray	= reinterpret_cast<dynamic_data*>(value_.pintVal);
    }
    break;
    default: {
        pArray	= new dynamic_data();

        value_.Clear();
        value_.vt		= VARTYPE(VALUE_OBJECT);
        value_.pintVal	= reinterpret_cast<int*>(pArray);
    }
    }
    return	*pArray;
}

void dynamic_value::internal_clear_data() {
    switch(value_.vt) {
    case VALUE_ARRAY:
    case VALUE_OBJECT: {
        delete reinterpret_cast<dynamic_data*>(value_.pintVal);
        value_.vt		= VT_EMPTY;
        value_.pintVal	= NULL;
    }
    break;
    default: {
        value_.Clear();
    }
    }
}

dynamic_value dynamic_value::operator [] (const int index) {
    dynamic_data&	data	= internal_as_data();
    value_.vt	= VALUE_ARRAY;
    return	data[index];
}

dynamic_value dynamic_value::operator [] (const std::string& key) {
    dynamic_data&	data	= internal_as_data();
    value_.vt	= VALUE_OBJECT;
    return	data[key];
}

dynamic_data::~dynamic_data() {
    internal_clear();
}

void dynamic_data::clear_as_array() {
    internal_clear();
    value_.vt	= VALUE_ARRAY;
}

void dynamic_data::clear_as_object() {
    internal_clear();
    value_.vt	= VALUE_OBJECT;
}

void dynamic_data::internal_clear() {
    switch(value_.vt) {
    case VALUE_ARRAY:
        internal_clear_array();
        break;
    case VALUE_OBJECT:
        internal_clear_object();
        break;
    default:
        value_.Clear();
        break;
    }
}

void dynamic_data::internal_clear_array() {
    if(VALUE_ARRAY == value_.vt) {
        ARRAY_TYPE::iterator	it		= array_.begin();
        ARRAY_TYPE::iterator	it_end	= array_.end();
        for(; it != it_end; ++it) {
            switch(it->second.vt) {
            case VALUE_ARRAY:
            case VALUE_OBJECT: {
                delete reinterpret_cast<dynamic_data*>(it->second.pintVal);
                it->second.vt		= VT_EMPTY;
                it->second.pintVal	= NULL;
            }
            break;
            }
        }

        array_.clear();

        value_.vt		= VT_EMPTY;
        value_.pintVal	= NULL;
    }
}

void dynamic_data::internal_clear_object() {
    if(VALUE_OBJECT == value_.vt) {
        OBJECT_TYPE::iterator	it		= object_.begin();
        OBJECT_TYPE::iterator	it_end	= object_.end();
        for(; it != it_end; ++it) {
            switch(it->second.vt) {
            case VALUE_ARRAY:
            case VALUE_OBJECT: {
                delete reinterpret_cast<dynamic_data*>(it->second.pintVal);
                it->second.vt		= VT_EMPTY;
                it->second.pintVal	= NULL;
            }
            break;
            }
        }

        object_.clear();

        value_.vt		= VT_EMPTY;
        value_.pintVal	= NULL;
    }
}

dynamic_value dynamic_data::operator [] (const int index) {
    switch(value_.vt) {
    case VALUE_ARRAY:
        break;
    case VALUE_OBJECT:
        internal_clear_object();
        value_.vt = VARTYPE(VALUE_ARRAY);
        break;
    default:
        value_.Clear();
        value_.vt = VARTYPE(VALUE_ARRAY);
        break;
    }

    return	dynamic_value(array_[index]);
}

dynamic_value dynamic_data::operator [] (const std::string& key) {
    switch(value_.vt) {
    case VALUE_ARRAY:
        internal_clear_array();
        value_.vt = VARTYPE(VALUE_OBJECT);
        break;
    case VALUE_OBJECT:
        break;
    default:
        value_.Clear();
        value_.vt = VARTYPE(VALUE_OBJECT);
        break;
    }

    return	dynamic_value(object_[key]);
}

bool dynamic_data::traverse(dynamic_data::item_iterator func) {
    if(!func) {
        return	false;
    }

    switch(type()) {
    case VALUE_ARRAY: {
        ARRAY_TYPE::iterator	it		= array_.begin();
        ARRAY_TYPE::iterator	it_end	= array_.end();
        for(; it != it_end; ++it) {
            func(it->first, NULL, dynamic_value(it->second));
        }
    }
    break;
    case VALUE_OBJECT: {
        OBJECT_TYPE::iterator	it		= object_.begin();
        OBJECT_TYPE::iterator	it_end	= object_.end();
        for(; it != it_end; ++it) {
            func(-1, it->first.c_str(), dynamic_value(it->second));
        }
    }
    break;
    default:
        return	false;
    }
    return	true;
}

dynamic_data::traverse_event dynamic_data::internal_make_traverse_event(
    TRAVERSE_EVENT_TYPE type, const dynamic_value& item, const std::string& key, int index, size_t remain_siblings) {
    traverse_event	event	= {type, item, key, index, remain_siblings};
    return	event;
}

void dynamic_data::traverse(traverse_iterator func) {
    if(!func) {
        return;
    }

    variant_t	vDummy;
    const	std::string		sDummy;
    const	dynamic_value	dDummy(vDummy);

    func(
        internal_make_traverse_event(TRAVERSE_BEGIN,	dDummy,	sDummy,	-1, 0)
    );
    internal_traverse(
        internal_make_traverse_event(TRAVERSE_ITEM,	dDummy,	sDummy,	-1, 0)
        , func
    );
    func(
        internal_make_traverse_event(TRAVERSE_END,	dDummy,	sDummy,	-1, 0)
    );
}

void dynamic_data::internal_traverse(const traverse_event& curr_stage, traverse_iterator func) {
    const	std::string		sDummy;
    switch(type()) {
    case VALUE_ARRAY: {
        func(
            internal_make_traverse_event(curr_stage.type,	dynamic_value(value_),	curr_stage.key,	curr_stage.index, curr_stage.remain_siblings)
        );

        ARRAY_TYPE::iterator	it		= array_.begin();
        ARRAY_TYPE::iterator	it_end	= array_.end();
        for(size_t nSize = array_.size() - 1; it != it_end; ++it, --nSize) {
            switch(it->second.vt) {
            case VALUE_ARRAY:
            case VALUE_OBJECT: {
                reinterpret_cast<dynamic_data*>(it->second.pintVal)->internal_traverse(
                    internal_make_traverse_event(TRAVERSE_ARRAY_ITEM, dynamic_value(it->second), sDummy, it->first, nSize)
                    , func
                );
            }
            break;
            default: {
                func(
                    internal_make_traverse_event(TRAVERSE_ARRAY_ITEM, dynamic_value(it->second), sDummy, it->first, nSize)
                );
            }
            break;
            }
        }
        func(
            internal_make_traverse_event(TRAVERSE_GROUP_END,	dynamic_value(value_),	curr_stage.key,	curr_stage.index, curr_stage.remain_siblings)
        );
    }
    break;
    case VALUE_OBJECT: {
        func(
            internal_make_traverse_event(curr_stage.type,	dynamic_value(value_),	curr_stage.key,	curr_stage.index, curr_stage.remain_siblings)
        );
        OBJECT_TYPE::iterator	it		= object_.begin();
        OBJECT_TYPE::iterator	it_end	= object_.end();
        for(size_t nSize = object_.size() - 1; it != it_end; ++it, --nSize) {
            switch(it->second.vt) {
            case VALUE_ARRAY:
            case VALUE_OBJECT: {
                reinterpret_cast<dynamic_data*>(it->second.pintVal)->internal_traverse(
                    internal_make_traverse_event(TRAVERSE_OBJECT_ITEM, dynamic_value(it->second), it->first, -1, nSize)
                    , func
                );
            }
            break;
            default: {
                func(
                    internal_make_traverse_event(TRAVERSE_OBJECT_ITEM, dynamic_value(it->second), it->first, -1, nSize)
                );
            }
            break;
            }
        }
        func(
            internal_make_traverse_event(TRAVERSE_GROUP_END,	dynamic_value(value_),	curr_stage.key,	curr_stage.index, curr_stage.remain_siblings)
        );
    }
    break;
    default: {
        func(
            internal_make_traverse_event(curr_stage.type,	dynamic_value(value_), curr_stage.key,	curr_stage.index, curr_stage.remain_siblings)
        );
    }
    break;
    }
}
