#ifndef DYNAMICAL_ARRAY_H
#define DYNAMICAL_ARRAY_H

/* 使用 void* 实现泛型，从而实现类似 vector 的功能
 * 这里只实现了简单的动态数组
 * 利用摊销的思想，每次插入的时间复杂度为 O(1)
 *
 * 使用方法：将该类作为类似基类的角色，然后通过 typedef 实现具体类型的动态数组
 * 例如：
 *     typedef struct DynamicalArray IntArray;
 *     IntArray* newIntArray() {
 *        return (IntArray*)newDynamicalArray(sizeof(int));
 *     }
 *     ...
 */

#define DYNAMICAL_ARRAY_INIT_CAPACITY 8

struct DynamicalArray {
    void*  data;
    int    size;
    int    capacity;
    int    dataTypeSize;
};

/**
 * @brief  新建动态数组
 * @param  dataTypeSize 数组所存数据的类型所占字节数
 * @return 指向该动态数组的指针
 */
struct DynamicalArray* newDynamicalArray(int dataTypeSize);

/**
 * @brief  释放动态数组占用的空间
 * @param  array 待释放的数组指针
 * @return void
 */
void freeDynamicalArray(struct DynamicalArray* array);

/**
 * @brief  向动态数组中添加一个元素
 * @param  array   待添加元素的数组指针
 * @param  element 待添加的元素
 * @return void
 */
void addElement(struct DynamicalArray* array, void* element);

/**
 * @brief  获取数组中指定位置的元素
 * @param  array 动态数组指针
 * @param  index 索引，从 0 开始
 * @return void* 指向获取到的元素首地址
 */
void* getElement(struct DynamicalArray* array, int index);

#endif
