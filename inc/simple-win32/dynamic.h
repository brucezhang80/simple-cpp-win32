#ifndef DYNAMIC_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2
#define DYNAMIC_H_WIN_66BC65DB_AFF6_43C8_8654_D1A2801635E2

//
//	NEED LIBRARY:	comsupp.lib	comsuppw.lib/comsuppwd.lib
//

#include <vector>
#include <deque>
#include <iosfwd>
#include <map>
#include <string>
#include <comutil.h>
#include "simple/delegate.h"
#include "simple/type.h"

class	DynamicValue;
class	DynamicData;
//
//	动态数据项，代表结构中的项的值
//
class	DynamicValue {
public:
    explicit		DynamicValue(variant_t& value):value_(value)	{}

public:
    enum	VALUE_TYPE {
        VALUE_EMPTY		= VT_EMPTY,			//	空，什么也没有
        VALUE_BOOL		= VT_BOOL,
        VALUE_INT8		= VT_I1,
        VALUE_UINT8		= VT_UI1,
        VALUE_INT16		= VT_I2,
        VALUE_UINT16	= VT_UI2,
        VALUE_INT32		= VT_I4,
        VALUE_UINT32	= VT_UI4,
        VALUE_INT		= VT_INT,
        VALUE_UINT		= VT_UINT,
        VALUE_FLOAT		= VT_R4,
        VALUE_DOUBLE	= VT_R8,
        VALUE_STRING	= VT_BSTR,
        VALUE_ARRAY		= VT_CARRAY,
        VALUE_OBJECT	= VT_USERDEFINED,
    };

public:
    VALUE_TYPE		type()const {
        return VALUE_TYPE(value_.vt);
    }

    void			clear() {
        do_clear_data();
    }
    void			clear_as_array();
    void			clear_as_object();

    DynamicValue	operator [](const int index);
    DynamicValue	operator [](const std::string& key);

    template<typename Type>
    void		operator=(const Type& value) {
        set_value(value);
    }

    bool			as_bool()const {
        return value_;
    }
    signed char		as_char()const {
        return char(value_);
    }
    unsigned char	as_byte()const {
        return value_;
    }
    signed short	as_short()const {
        return value_;
    }
    unsigned short	as_ushort()const {
        return value_;
    }
    signed int		as_int()const {
        return value_;
    }
    unsigned int	as_uint()const {
        return value_;
    }
    signed long		as_long()const {
        return value_;
    }
    unsigned long	as_ulong()const {
        return value_;
    }
    float			as_float()const {
        return value_;
    }
    double			as_double()const {
        return value_;
    }
    _bstr_t			as_string()const {
        return value_;
    }
    DynamicData*	as_data()const;

    template<typename Type>
    bool		value(Type& value) {
        switch(value_.vt) {
        case VALUE_ARRAY:
        case VALUE_OBJECT: {
            return DynamicData_ai_v2oi(value, *this);;
        }
        break;
        default: {
            return	false;
        }
        }
    }
    bool 			value(bool& value) {
        value = value_;
        return true;
    }
    bool 			value(signed char& value) {
        value = char(value_);
        return true;
    }
    bool 			value(unsigned char& value) {
        value = value_;
        return true;
    }
    bool 			value(signed short& value) {
        value = value_;
        return true;
    }
    bool 			value(unsigned short& value) {
        value = value_;
        return true;
    }
    bool 			value(signed int& value) {
        value = value_;
        return true;
    }
    bool 			value(unsigned int& value) {
        value = value_;
        return true;
    }
    bool 			value(signed long& value) {
        value = value_;
        return true;
    }
    bool 			value(unsigned long& value) {
        value = value_;
        return true;
    }
    bool 			value(float& value) {
        value = value_;
        return true;
    }
    bool 			value(double& value) {
        value = value_;
        return true;
    }
    bool 			value(_bstr_t& value) {
        value = value_;
        return true;
    }
    bool			value(std::string& val) {
        val = _bstr_t(value_);
        return true;
    }
    bool			value(std::wstring& val) {
        val = _bstr_t(value_);
        return true;
    }

    template<typename ObjectType>
    void		set_value(const ObjectType& obj) {
        DynamicData_oi2v(obj, *this);
    }
    void 			set_value(bool value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(signed char value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(unsigned char value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(signed short value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(unsigned short value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(signed int value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(unsigned int value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(signed long value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(unsigned long value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(float value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(double value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(const char* value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(const std::string& value) {
        do_clear_data();
        value_ = value.c_str();
    }
    void 			set_value(const wchar_t* value) {
        do_clear_data();
        value_ = value;
    }
    void 			set_value(const std::wstring& value) {
        do_clear_data();
        value_ = value.c_str();
    }
    void 			set_value(const _bstr_t& value) {
        do_clear_data();
        value_ = value;
    }

protected:
    virtual	DynamicData&	do_as_data();

private:
    void			do_clear_data();

private:
    variant_t&	value_;
};

//
//	动态数据结构
//
class	DynamicData	: public DynamicValue {
public:
    explicit		DynamicData():DynamicValue(value_)	{}
    inline			~DynamicData();

public:
    void			clear() {
        do_clear();
    }
    void			clear_as_array();
    void			clear_as_object();

    DynamicValue	operator [](const int index);
    DynamicValue	operator [](const std::string& key);

    template<typename Type>
    void		operator=(const Type& value) {
        set_value(value);
    }

private:
    typedef	std::map<int, variant_t>			ARRAY_TYPE;
    typedef	std::map<std::string, variant_t>	OBJECT_TYPE;

public:
    //
    //	结构遍历，可通过此方法生成 XML/JSON/LUA 等格式
    //

    //	遍历节点类型
    enum	TRAVERSE_EVENT_TYPE {
        TRAVERSE_BEGIN,				//	遍历开始
        TRAVERSE_ITEM,				//	普通项
        TRAVERSE_ARRAY_ITEM,		//	数组项
        TRAVERSE_OBJECT_ITEM,		//	对象成员项
        TRAVERSE_GROUP_END,			//	对象或数组结束事件
        TRAVERSE_END,				//	遍历结束
    };

    //	遍历节点事件
    struct	TraverseEvent {
        TRAVERSE_EVENT_TYPE		type;			//	traverse event type
        const DynamicValue&		item;			//	current	item, root if TRAVERSE_BEGIN/TRAVERSE_END
        const std::string&		key;			//	used by	VALUE_OBJECT item
        int						index;			//	used by VALUE_ARRAY item
        size_t					remain_siblings;//	array/object remain sibling items sum.
    };

    //	递归遍历处理函数
    typedef	delegate<void(const TraverseEvent& item)>	TraverseIterator;
    //	数组遍历
    typedef	delegate<void(int nIndex, const char* sKey, DynamicValue& item)>	ItemIterator;

    //	递归遍历方法
    void			traverse(TraverseIterator func);
    // 仅遍历直接子项
    bool			traverse(ItemIterator func);

protected:
    virtual	DynamicData&	do_as_data() {
        return *this;
    }

private:
    void			do_clear();
    void			do_clear_array();
    void			do_clear_object();

    void			do_traverse(const TraverseEvent& curr_stage, TraverseIterator func);
    TraverseEvent	do_make_traverse_event(TRAVERSE_EVENT_TYPE type_, const DynamicValue& item_, const std::string& key_ = "", int index_ = -1, size_t remain_siblings = 0);

private:
    variant_t	value_;
    ARRAY_TYPE	array_;
    OBJECT_TYPE	object_;

private:
    DynamicData(const DynamicData&);
    const DynamicData&	operator=(const DynamicData&);
};

template<typename ObjectType>
inline	void	DynamicData_o2d(const ObjectType& obj, DynamicData& data) {
    data.set_value(obj);
}

template<typename ObjectType>
inline	bool	DynamicData_d2o(ObjectType& obj, DynamicData& data) {
    return	DynamicData_ai_v2oi(obj, data);
}

// STL container helper.
template<typename ArrayType>
void	DynamicData_oi2v(const ArrayType& array, DynamicValue& value) {
    value.clear_as_array();

    int	size						= (int)array.size();
    ArrayType::const_iterator it	= array.begin();
    for(int i = 0; i < size; ++i, ++it) {
        value[i].set_value(*it);
    }
}

template<typename ArrayType>
struct	DynamicData_ai_traverser {
    ArrayType&	array;
    void		TraverseHandler(int nIndex, const char* sKey, DynamicValue& item) {
        DynamicData_ai_v2ai(item, array, nIndex);
    }
};
template<typename ArrayType>
bool	DynamicData_ai_v2oi(ArrayType& array, DynamicValue& value) {
    array.clear();

    if(DynamicValue::VALUE_ARRAY != value.type()) {
        return	false;
    }

    typedef	DynamicData_ai_traverser<ArrayType>	Iterator;
    Iterator	it = {array};
    value.as_data()->traverse(bind(Iterator::TraverseHandler, &it));

    return	true;
}

template<typename ArrayType>
bool	DynamicData_ai_v2ai(DynamicValue& value, ArrayType& array, size_t nIndex) {
    ArrayType::value_type	t;
    if(value.value(t)) {
        array.insert(array.end(), t);
        return	true;
    }

    return	false;
}

template<class KEY, class VALUE>
bool	DynamicData_ai_v2ai(DynamicValue& value, std::map<KEY,VALUE>& array, size_t nIndex) {
    std::pair<KEY, VALUE>	t;
    if(value.value(t)) {
        array.insert(array.end(), t);
        return	true;
    }

    return	false;
}

// pair helper.
template<class KEY, class VALUE>
void	DynamicData_oi2v(const std::pair<KEY,VALUE>& obj, DynamicValue& value) {
    value.clear_as_array();
    value[1].set_value(obj.first);
    value[2].set_value(obj.second);
}
template<class KEY, class VALUE>
bool	DynamicData_ai_v2oi(std::pair<KEY,VALUE>& obj, DynamicValue& value) {
    value[1].value(obj.first);
    value[2].value(obj.second);

    return	true;
}

class	buffer;
void DynamicData_oi2v(const buffer& obj, DynamicValue& data);
bool DynamicData_ai_v2oi(buffer& obj, DynamicValue& data);


/////////////////////////////////////////////////////////////////
//
//	json support for dynamic data/value
//
bool	DynamicValue_ToJSON			(std::ostream& os,		const DynamicValue& value);
bool	DynamicData_ToJSON			(std::ostream& os,		DynamicData& value);
bool	DynamicData_FromJSON		(DynamicData& data,		const std::string& pszJSON);


/////////////////////////////////////////////////////////////////
//
//	dynamic functions
//
class	DynamicFunction {
public:
    bool			call_function(const char* func_name, const std::deque<DynamicData*>& args) {
        ret_.clear();
        return	do_call_function(func_name, args, ret_);
    }
    template<typename ValueType>
    bool			get_return_value(ValueType& value) {
        return	ret_.value(value);
    }
    DynamicData&	get_return_value() {
        return	ret_;
    }


public:
    bool			call(const char* pszFunction) {
        std::deque<DynamicData*>	args;
        return	call_function(pszFunction, args);
    }
    template<typename Arg1>
    bool			call(const char* pszFunction, const Arg1& a1) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        return	call_function(pszFunction, args);
    }
    template<typename Arg1, typename Arg2>
    bool			call(const char* pszFunction, const Arg1& a1, const Arg2& a2) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        return	call_function(pszFunction, args);
    }
    template<typename Arg1, typename Arg2, typename Arg3>
    bool			call(const char* pszFunction, const Arg1& a1, const Arg2& a2, const Arg3& a3) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        return	call_function(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4>
    bool			call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        return	call_function(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5>
    bool			call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        return	call_function(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    bool			call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        PA(args, a6);
        return	call_function(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    bool			call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        PA(args, a6);
        PA(args, a7);
        return	call_function(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
    bool			call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        PA(args, a6);
        PA(args, a7);
        PA(args, a8);
        return	call_function(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
    bool			call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        PA(args, a6);
        PA(args, a7);
        PA(args, a8);
        PA(args, a9);
        return	call_function(pszFunction, args);
    }
    template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
    bool			call(const char* pszFunction, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10) {
        std::deque<DynamicData*>	args;
        PA(args, a1);
        PA(args, a2);
        PA(args, a3);
        PA(args, a4);
        PA(args, a5);
        PA(args, a6);
        PA(args, a7);
        PA(args, a8);
        PA(args, a9);
        PA(args, a10);
        return	call_function(pszFunction, args);
    }

protected:
    virtual	bool	do_call_function(const char* func_name, const std::deque<DynamicData*>& args, DynamicData& ret) {
        return	false;
    }

    inline	void	do_push_augument(std::deque<DynamicData*>& args, const DynamicData& arg) {
        args.push_back(const_cast<DynamicData*>(&arg));
    }
    template<typename ArgType>
    inline	void	do_push_augument(std::deque<DynamicData*>& args, const ArgType& arg) {
        int	nIndex	= args.size();
        args_[nIndex]	= arg;
        args.push_back(&args_[nIndex]);
    }
    template<typename ArgType>
    inline	void	PA(std::deque<DynamicData*>& args, const ArgType& arg) {
        do_push_augument(args, arg);
    }

protected:
    DynamicData		ret_;
    DynamicData		args_[10];
};

#define DYNAMIC_FUNCTIONS_BEGIN(theClass) \
protected:\
typedef	theClass	DynamicFunction__THIS_CLASS;\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 0) return false;\
	return(this->*func)(ret);\
}\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(DynamicData&, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 1) return false;\
	return(this->*func)(*args[0], ret);\
}\
template<typename A1>\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(A1, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 1) return false;\
	typename type_inference<A1>::undecorated_type	a1;	args[0]->value(a1);\
	return(this->*func)(a1, ret);\
}\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(DynamicData&, DynamicData&, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 2) return false;\
	return(this->*func)(*args[0], *args[1], ret);\
}\
template<typename A1, typename A2>\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(A1, A2, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 2) return false;\
	typename type_inference<A1>::undecorated_type	a1;	args[0]->value(a1);\
	typename type_inference<A2>::undecorated_type	a2;	args[1]->value(a2);\
	return(this->*func)(a1, a2, ret);\
}\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(DynamicData&, DynamicData&, DynamicData&, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 3) return false;\
	return(this->*func)(*args[0], *args[1], *args[2], ret);\
}\
template<typename A1, typename A2, typename A3>\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(A1, A2, A3, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 3) return false;\
	typename type_inference<A1>::undecorated_type	a1;	args[0]->value(a1);\
	typename type_inference<A2>::undecorated_type	a2;	args[1]->value(a2);\
	typename type_inference<A3>::undecorated_type	a3;	args[2]->value(a3);\
	return(this->*func)(a1, a2, a3, ret);\
}\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 4) return false;\
	return(this->*func)(*args[0], *args[1], *args[2], *args[3], ret);\
}\
template<typename A1, typename A2, typename A3, typename A4>\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(A1, A2, A3, A4, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 4) return false;\
	typename type_inference<A1>::undecorated_type	a1;	args[0]->value(a1);\
	typename type_inference<A2>::undecorated_type	a2;	args[1]->value(a2);\
	typename type_inference<A3>::undecorated_type	a3;	args[2]->value(a3);\
	typename type_inference<A4>::undecorated_type	a4;	args[3]->value(a4);\
	return(this->*func)(a1, a2, a3, a4, ret);\
}\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 5) return false;\
	return(this->*func)(*args[0], *args[1], *args[2], *args[3], *args[4], ret);\
}\
template<typename A1, typename A2, typename A3, typename A4, typename A5>\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(A1, A2, A3, A4, A5, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 5) return false;\
	typename type_inference<A1>::undecorated_type	a1;	args[0]->value(a1);\
	typename type_inference<A2>::undecorated_type	a2;	args[1]->value(a2);\
	typename type_inference<A3>::undecorated_type	a3;	args[2]->value(a3);\
	typename type_inference<A4>::undecorated_type	a4;	args[3]->value(a4);\
	typename type_inference<A5>::undecorated_type	a5;	args[4]->value(a5);\
	return(this->*func)(a1, a2, a3, a4, a5, ret);\
}\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 6) return false;\
	return(this->*func)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], ret);\
}\
template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(A1, A2, A3, A4, A5, A6, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 6) return false;\
	typename type_inference<A1>::undecorated_type	a1;	args[0]->value(a1);\
	typename type_inference<A2>::undecorated_type	a2;	args[1]->value(a2);\
	typename type_inference<A3>::undecorated_type	a3;	args[2]->value(a3);\
	typename type_inference<A4>::undecorated_type	a4;	args[3]->value(a4);\
	typename type_inference<A5>::undecorated_type	a5;	args[4]->value(a5);\
	typename type_inference<A6>::undecorated_type	a6;	args[5]->value(a6);\
	return(this->*func)(a1, a2, a3, a4, a5, a6, ret);\
}\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 7) return false;\
	return(this->*func)(*args[0], *args[1], *args[2], *args[3], *args[4], *args[5], *args[6], ret);\
}\
template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(A1, A2, A3, A4, A5, A6, A7, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 7) return false;\
	typename type_inference<A1>::undecorated_type	a1;	args[0]->value(a1);\
	typename type_inference<A2>::undecorated_type	a2;	args[1]->value(a2);\
	typename type_inference<A3>::undecorated_type	a3;	args[2]->value(a3);\
	typename type_inference<A4>::undecorated_type	a4;	args[3]->value(a4);\
	typename type_inference<A5>::undecorated_type	a5;	args[4]->value(a5);\
	typename type_inference<A6>::undecorated_type	a6;	args[5]->value(a6);\
	typename type_inference<A7>::undecorated_type	a7;	args[6]->value(a7);\
	return(this->*func)(a1, a2, a3, a4, a5, a6, a7, ret);\
}\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 8) return false;\
	return(this->*func)(*args[0],*args[1],*args[2],*args[3],*args[4],*args[5],*args[6],*args[7], ret);\
}\
template<typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8>\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(A1,A2,A3,A4,A5,A6,A7,A8, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 8) return false;\
	typename type_inference<A1>::undecorated_type	a1;	args[0]->value(a1);\
	typename type_inference<A2>::undecorated_type	a2;	args[1]->value(a2);\
	typename type_inference<A3>::undecorated_type	a3;	args[2]->value(a3);\
	typename type_inference<A4>::undecorated_type	a4;	args[3]->value(a4);\
	typename type_inference<A5>::undecorated_type	a5;	args[4]->value(a5);\
	typename type_inference<A6>::undecorated_type	a6;	args[5]->value(a6);\
	typename type_inference<A7>::undecorated_type	a7;	args[6]->value(a7);\
	typename type_inference<A8>::undecorated_type	a8;	args[7]->value(a8);\
	return(this->*func)(a1,a2,a3,a4,a5,a6,a7,a8, ret);\
}\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 9) return false;\
	return(this->*func)(*args[0],*args[1],*args[2],*args[3],*args[4],*args[5],*args[6],*args[7],*args[8], ret);\
}\
template<typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8,typename A9>\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(A1,A2,A3,A4,A5,A6,A7,A8,A9, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 9) return false;\
	typename type_inference<A1>::undecorated_type	a1;	args[0]->value(a1);\
	typename type_inference<A2>::undecorated_type	a2;	args[1]->value(a2);\
	typename type_inference<A3>::undecorated_type	a3;	args[2]->value(a3);\
	typename type_inference<A4>::undecorated_type	a4;	args[3]->value(a4);\
	typename type_inference<A5>::undecorated_type	a5;	args[4]->value(a5);\
	typename type_inference<A6>::undecorated_type	a6;	args[5]->value(a6);\
	typename type_inference<A7>::undecorated_type	a7;	args[6]->value(a7);\
	typename type_inference<A8>::undecorated_type	a8;	args[7]->value(a8);\
	typename type_inference<A9>::undecorated_type	a9;	args[8]->value(a9);\
	return(this->*func)(a1,a2,a3,a4,a5,a6,a7,a8,a9, ret);\
}\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData&, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 10) return false;\
	return(this->*func)(*args[0],*args[1],*args[2],*args[3],*args[4],*args[5],*args[6],*args[7],*args[8],*args[9], ret);\
}\
template<typename A1,typename A2,typename A3,typename A4,typename A5,typename A6,typename A7,typename A8,typename A9,typename A10>\
bool	invoke_dynamic_function(\
			bool (theClass::*func)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10, DynamicData& ret),\
			const std::deque<DynamicData*>& args, DynamicData& ret)\
{\
	if(args.size() != 10) return false;\
	typename type_inference<A1>::undecorated_type	a1;	args[0]->value(a1);\
	typename type_inference<A2>::undecorated_type	a2;	args[1]->value(a2);\
	typename type_inference<A3>::undecorated_type	a3;	args[2]->value(a3);\
	typename type_inference<A4>::undecorated_type	a4;	args[3]->value(a4);\
	typename type_inference<A5>::undecorated_type	a5;	args[4]->value(a5);\
	typename type_inference<A6>::undecorated_type	a6;	args[5]->value(a6);\
	typename type_inference<A7>::undecorated_type	a7;	args[6]->value(a7);\
	typename type_inference<A8>::undecorated_type	a8;	args[7]->value(a8);\
	typename type_inference<A9>::undecorated_type	a9;	args[8]->value(a9);\
	typename type_inference<A10>::undecorated_type	a10;args[9]->value(a10);\
	return(this->*func)(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10, ret);\
}\
virtual	bool	do_call_function(const char* func_name, const std::deque<DynamicData*>& args, DynamicData& ret)\
{

#define	DYNAMIC_FUNCTIONS_END()		\
	return __super::do_call_function(func_name, args, ret);\
 }

#define DYNAMICFUNCTION_ITEM(funcName, funcptr) \
if(0 == strcmp(func_name, funcName))\
{ \
	return invoke_dynamic_function(&DynamicFunction__THIS_CLASS:: funcptr, args, ret);\
}

#endif
