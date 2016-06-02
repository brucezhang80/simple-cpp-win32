#include <cassert>
#include "dynamic.h"

void DynamicValue::clear_as_array() {
    do_as_data().clear_as_array();
    value_.vt	= VALUE_ARRAY;
}

void DynamicValue::clear_as_object() {
    do_as_data().clear_as_object();
    value_.vt	= VALUE_OBJECT;
}

DynamicData* DynamicValue::as_data()const {
    switch(value_.vt) {
    case VALUE_ARRAY:
    case VALUE_OBJECT: {
        return reinterpret_cast<DynamicData*>(value_.pintVal);
    }
    break;
    }

    return	NULL;
}

DynamicData& DynamicValue::do_as_data() {
    DynamicData*	pArray	= NULL;
    switch(value_.vt) {
    case VALUE_ARRAY:
    case VALUE_OBJECT: {
        pArray	= reinterpret_cast<DynamicData*>(value_.pintVal);
    }
    break;
    default: {
        pArray	= new DynamicData();

        value_.Clear();
        value_.vt		= VARTYPE(VALUE_OBJECT);
        value_.pintVal	= reinterpret_cast<int*>(pArray);
    }
    }
    return	*pArray;
}

void DynamicValue::do_clear_data() {
    switch(value_.vt) {
    case VALUE_ARRAY:
    case VALUE_OBJECT: {
        delete reinterpret_cast<DynamicData*>(value_.pintVal);
        value_.vt		= VT_EMPTY;
        value_.pintVal	= NULL;
    }
    break;
    default: {
        value_.Clear();
    }
    }
}

DynamicValue DynamicValue::operator [] (const int index) {
    DynamicData&	data	= do_as_data();
    value_.vt	= VALUE_ARRAY;
    return	data[index];
}

DynamicValue DynamicValue::operator [] (const std::string& key) {
    DynamicData&	data	= do_as_data();
    value_.vt	= VALUE_OBJECT;
    return	data[key];
}

DynamicData::~DynamicData() {
    do_clear();
}

void DynamicData::clear_as_array() {
    do_clear();
    value_.vt	= VALUE_ARRAY;
}

void DynamicData::clear_as_object() {
    do_clear();
    value_.vt	= VALUE_OBJECT;
}

void DynamicData::do_clear() {
    switch(value_.vt) {
    case VALUE_ARRAY:
        do_clear_array();
        break;
    case VALUE_OBJECT:
        do_clear_object();
        break;
    default:
        value_.Clear();
        break;
    }
}

void DynamicData::do_clear_array() {
    if(VALUE_ARRAY == value_.vt) {
        ARRAY_TYPE::iterator	it		= array_.begin();
        ARRAY_TYPE::iterator	it_end	= array_.end();
        for(; it != it_end; ++it) {
            switch(it->second.vt) {
            case VALUE_ARRAY:
            case VALUE_OBJECT: {
                delete reinterpret_cast<DynamicData*>(it->second.pintVal);
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

void DynamicData::do_clear_object() {
    if(VALUE_OBJECT == value_.vt) {
        OBJECT_TYPE::iterator	it		= object_.begin();
        OBJECT_TYPE::iterator	it_end	= object_.end();
        for(; it != it_end; ++it) {
            switch(it->second.vt) {
            case VALUE_ARRAY:
            case VALUE_OBJECT: {
                delete reinterpret_cast<DynamicData*>(it->second.pintVal);
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

DynamicValue DynamicData::operator [] (const int index) {
    switch(value_.vt) {
    case VALUE_ARRAY:
        break;
    case VALUE_OBJECT:
        do_clear_object();
        value_.vt = VARTYPE(VALUE_ARRAY);
        break;
    default:
        value_.Clear();
        value_.vt = VARTYPE(VALUE_ARRAY);
        break;
    }

    return	DynamicValue(array_[index]);
}

DynamicValue DynamicData::operator [] (const std::string& key) {
    switch(value_.vt) {
    case VALUE_ARRAY:
        do_clear_array();
        value_.vt = VARTYPE(VALUE_OBJECT);
        break;
    case VALUE_OBJECT:
        break;
    default:
        value_.Clear();
        value_.vt = VARTYPE(VALUE_OBJECT);
        break;
    }

    return	DynamicValue(object_[key]);
}

bool DynamicData::traverse(DynamicData::ItemIterator func) {
    if(!func) {
        return	false;
    }

    switch(type()) {
    case VALUE_ARRAY: {
        ARRAY_TYPE::iterator	it		= array_.begin();
        ARRAY_TYPE::iterator	it_end	= array_.end();
        for(; it != it_end; ++it) {
            func(it->first, NULL, DynamicValue(it->second));
        }
    }
    break;
    case VALUE_OBJECT: {
        OBJECT_TYPE::iterator	it		= object_.begin();
        OBJECT_TYPE::iterator	it_end	= object_.end();
        for(; it != it_end; ++it) {
            func(-1, it->first.c_str(), DynamicValue(it->second));
        }
    }
    break;
    default:
        return	false;
    }
    return	true;
}

DynamicData::TraverseEvent DynamicData::do_make_traverse_event(
    TRAVERSE_EVENT_TYPE type, const DynamicValue& item, const std::string& key, int index, size_t remain_siblings) {
    TraverseEvent	event	= {type, item, key, index, remain_siblings};
    return	event;
}

void DynamicData::traverse(TraverseIterator func) {
    if(!func) {
        return;
    }

    variant_t	vDummy;
    const	std::string		sDummy;
    const	DynamicValue	dDummy(vDummy);

    func(
        do_make_traverse_event(TRAVERSE_BEGIN,	dDummy,	sDummy,	-1, 0)
    );
    do_traverse(
        do_make_traverse_event(TRAVERSE_ITEM,	dDummy,	sDummy,	-1, 0)
        , func
    );
    func(
        do_make_traverse_event(TRAVERSE_END,	dDummy,	sDummy,	-1, 0)
    );
}

void DynamicData::do_traverse(const TraverseEvent& curr_stage, TraverseIterator func) {
    const	std::string		sDummy;
    switch(type()) {
    case VALUE_ARRAY: {
        func(
            do_make_traverse_event(curr_stage.type,	DynamicValue(value_),	curr_stage.key,	curr_stage.index, curr_stage.remain_siblings)
        );

        ARRAY_TYPE::iterator	it		= array_.begin();
        ARRAY_TYPE::iterator	it_end	= array_.end();
        for(size_t nSize = array_.size() - 1; it != it_end; ++it, --nSize) {
            switch(it->second.vt) {
            case VALUE_ARRAY:
            case VALUE_OBJECT: {
                reinterpret_cast<DynamicData*>(it->second.pintVal)->do_traverse(
                    do_make_traverse_event(TRAVERSE_ARRAY_ITEM, DynamicValue(it->second), sDummy, it->first, nSize)
                    , func
                );
            }
            break;
            default: {
                func(
                    do_make_traverse_event(TRAVERSE_ARRAY_ITEM, DynamicValue(it->second), sDummy, it->first, nSize)
                );
            }
            break;
            }
        }
        func(
            do_make_traverse_event(TRAVERSE_GROUP_END,	DynamicValue(value_),	curr_stage.key,	curr_stage.index, curr_stage.remain_siblings)
        );
    }
    break;
    case VALUE_OBJECT: {
        func(
            do_make_traverse_event(curr_stage.type,	DynamicValue(value_),	curr_stage.key,	curr_stage.index, curr_stage.remain_siblings)
        );
        OBJECT_TYPE::iterator	it		= object_.begin();
        OBJECT_TYPE::iterator	it_end	= object_.end();
        for(size_t nSize = object_.size() - 1; it != it_end; ++it, --nSize) {
            switch(it->second.vt) {
            case VALUE_ARRAY:
            case VALUE_OBJECT: {
                reinterpret_cast<DynamicData*>(it->second.pintVal)->do_traverse(
                    do_make_traverse_event(TRAVERSE_OBJECT_ITEM, DynamicValue(it->second), it->first, -1, nSize)
                    , func
                );
            }
            break;
            default: {
                func(
                    do_make_traverse_event(TRAVERSE_OBJECT_ITEM, DynamicValue(it->second), it->first, -1, nSize)
                );
            }
            break;
            }
        }
        func(
            do_make_traverse_event(TRAVERSE_GROUP_END,	DynamicValue(value_),	curr_stage.key,	curr_stage.index, curr_stage.remain_siblings)
        );
    }
    break;
    default: {
        func(
            do_make_traverse_event(curr_stage.type,	DynamicValue(value_), curr_stage.key,	curr_stage.index, curr_stage.remain_siblings)
        );
    }
    break;
    }
}
