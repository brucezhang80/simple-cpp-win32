#include <cassert>
#include "dynamic.h"
#include "simple/buffer.h"

void dynamic_data_oi2v(const buffer& obj, dynamic_value& data) {
    data.clear_as_array();

    int	size = (int)obj.size();
    char* pData = (char*)obj.data();
    for(int i = 0; i < size; ++i) {
        data[i].set_value((unsigned char)(pData[i]));
    }
}

bool	dynamic_data_v2bi(dynamic_value& value, buffer& array, size_t nIndex) {
    BYTE nByte;
    if(value.value(nByte)) {
        array.write(&nByte, 1);
        return	true;
    }

    return	false;
}

struct	dynamic_data_bi_traverser {
    buffer&	array;
    void		traverse_handler(int nIndex, const char* sKey, dynamic_value& item) {
        dynamic_data_v2bi(item, array, nIndex);
    }
};

typedef	dynamic_data_bi_traverser	buffer_iterator;
bool dynamic_data_ai_v2oi(buffer& obj, dynamic_value& data) {
    buffer_iterator	it = {obj};
    data.as_data()->traverse(bind(&buffer_iterator::traverse_handler, &it));
    return true;
}
