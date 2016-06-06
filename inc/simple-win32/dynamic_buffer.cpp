#include <cassert>
#include "dynamic.h"
#include "simple/buffer.h"

void DynamicData_oi2v(const buffer& obj, DynamicValue& data) {
    data.clear_as_array();

    int	size = (int)obj.size();
    char* pData = (char*)obj.data();
    for(int i = 0; i < size; ++i) {
        data[i].set_value((unsigned char)(pData[i]));
    }
}

bool	DynamicData_v2bi(DynamicValue& value, buffer& array, size_t nIndex) {
    BYTE nByte;
    if(value.value(nByte)) {
        array.write(&nByte, 1);
        return	true;
    }

    return	false;
}

struct	DynamicData_bi_traverser {
    buffer&	array;
    void		traverse_handler(int nIndex, const char* sKey, DynamicValue& item) {
        DynamicData_v2bi(item, array, nIndex);
    }
};

typedef	DynamicData_bi_traverser	buffer_iterator;
bool DynamicData_ai_v2oi(buffer& obj, DynamicValue& data) {
    buffer_iterator	it = {obj};
    data.as_data()->traverse(bind(&buffer_iterator::traverse_handler, &it));
    return true;
}
