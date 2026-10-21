#ifndef MYTINYSTL_EXCEPTDEF_H
#define MYTINYSTL_EXCEPTDEF_H

/**
 * MyTinySTL 的调试和异常宏——定义了一组预处理宏，用于断言检查、抛异常
 */
#include <stdexcept>
#include <cassert>

namespace mystl
{
#define MYSTL_DEBUG(expr) \
 assert(expr)

#define THROW_LENGTH_ERROR_IF(expr,what) \ if ((expr)) throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr,what) \ if((expr)) throw std::runtime_error(what)

}


#endif //MYTINYSTL_EXCEPTDEF_H
