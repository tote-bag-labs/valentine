// Tote Bag Labs 2023

#include <type_traits>

#ifndef type_helpers_hpp
#define type_helpers_hpp

namespace tote_bag
{
namespace type_helpers
{
/** A a type that evaluates to false if instantiated.
 * 
 *  This is useful for static asserts that should only trigger based on conditions
 *  evaluated using template arguments at compile time.
 *  
 *  Bool cannot be used in this case, as the resulting expression will always be compiled 
 *  and hence cause an error, no matter whether the conditions for hitting the 
 *  assert have been met or not.
 * 
 *  Example:
 *
 *  template <typename T>
 *  void functionThatNeedsIntType()
 *  {
 *      if constexpr (std::is_same_v<T, int>)
 *      {
 *          // Do something.
 *      }
 *      else
 *      {
 *         // This will always cause a compilation error.
 *         //static_assert(false, "This type is not supported");
 * 
 *         // This will only cause an error if the template is instantiated with a type other than int,
 *         // causing this branch to be evaluated and the dependent_false to be instantiated.
 *         static_assert(dependent_false<T>::value, "This type is not supported");
 *      }
 *  }
 */
template <typename T>
struct dependent_false : std::false_type
{
};
}
}

#endif /* type_helpers_hpp */
