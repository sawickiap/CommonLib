#pragma once

/// Very fast algorithm for sorting array of float numbers.
/** The algorithm comes from article: \n
Radix Tricks, Michael Herf - http://stereopsis.com/radix.html
\param[in]  farray     Input array. Its contents will be destroyed and is undefined on exit. Must have "elements" elements.
\param[out] sorted     Output array, filled with sorted elements. Must have "elements" elements.
\param[in]  elements   Length of arrays.
*/
void float_radix_sort(float *farray, float *sorted, unsigned elements);
