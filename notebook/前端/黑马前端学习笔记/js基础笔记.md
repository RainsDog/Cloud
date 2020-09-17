# 内置对象

内置对象提供了一些常用的属性和方法，能够帮助我们快速开发

如Math、Date、Array、String等

## Math 数学对象

* Math不是构造函数，它提供静态的属性和方法

  | 属性、方法名          | 功能                                         |
  | --------------------- | -------------------------------------------- |
  | Math.PI               | 圆周率                                       |
  | Math.floor()          | 向下取整                                     |
  | Math.ceil()           | 向上取整                                     |
  | Math.round()          | 四舍五入版 就近取整   注意 -3.5   结果是  -3 |
  | Math.abs()            | 绝对值                                       |
  | Math.max()/Math.min() | 求最大和最小值                               |
  | Math.random()         | 获取范围在[0,1)内的随机值                    |

* 对于形参，如果能转换为数字，则转换，不能转换，结果就是NaN

### 求指定范围内的随机整数

```js
function getRandom(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min; 
}
```

## Date 日期对象

* Date是一个构造函数，使用前必须通过`new Date()`构造实例对象
* Data对象有日期和时间相关的数据和方法
* 使用举例：

```js
//获取当前的日期和时间
var now = new Date();
//获取指定日期或时间的对象
var date = new Date('2019/5/1');
```

* 如果创建实例时并未传入参数，则得到的日期对象是当前时间对应的日期对象
* 使用Date实例的方法和属性	

| 方法名      | 说明                      |
| ----------- | ------------------------- |
| getFullYear | 获取当年                  |
| getMonth    | 获取当月（0-11）          |
| getDate     | 获取当天                  |
| getDay      | 获取星期几（0周日-6周六） |
| getHours    | 获取当前小时              |
| getMinutes  | 获取当前分钟              |
| getSeconds  | 获取当前秒数              |

### 获取总的毫秒数

获取距离1970.1.1时间到现在的毫秒数，有三种方法

```js
// 实例化Date对象
var now = new Date();
// 1. 用于获取对象的原始值
console.log(now.valueOf())	
console.log(now.getTime())	
// 2. 简单写可以这么做
var now = + new Date();			
// 3. HTML5中提供的静态方法，有兼容性问题
var now = Date.now();
```

## 数组对象

创建数组对象有两种方式

* 字面量

```js
var arr = [1,"test",true];
```

* 构造函数

```js
//不传参数时，创建空数组
var arr1 = new Array();
//只传一个参数，该参数为数组长度
var arr2 = new Array(2);
//传入多个参数，这些参数为数组的元素
var arr3 = new Array(2,3,4);
```

### 检测对象是否为数组

* instanceof 运算符

* Array.isArray() 静态方法

```js
var arr = [1, 2, 3];
var arrf = 'abc123';
console.log(arr instanceof Array);
console.log(arrf instanceof Array);
console.log(Array.isArray(arr));
console.log(Array.isArray(arrf));
```

### 添加或删除数组元素

以下四种方法，均会改变原数组

| 方法名              | 说明                 | 返回值         |
| ------------------- | -------------------- | -------------- |
| push(参数1，...)    | 末尾添加元素         | 新的长度       |
| pop()               | 末尾删除**一个**元素 | 被删除的值     |
| unshift(参数1，...) | 开头增加元素         | 新的长度       |
| shift()             | 开头删除**一个**元素 | 第一个元素的值 |

### 数组排序

以下两种方法会改变原数组，并返回

| 方法名    | 说明                   | 返回值       |
| --------- | ---------------------- | ------------ |
| reverse() | 颠倒数组中元素的顺序   | 修改过的数组 |
| sort()    | 对数组中的元素进行排序 | 修改过的数组 |

注意：sort方法需要传入参数来设置升序、降序排序

- 如果传入“function(a,b){ return a-b;}”，则为升序
- 如果传入“function(a,b){ return b-a;}”，则为降序

### 数组索引

| 方法名        | 说明                                                         | 返回值         |
| ------------- | ------------------------------------------------------------ | -------------- |
| indexOf()     | 查找指定元素的索引，找到即返回，不存在则返回-1               | 元素索引号或-1 |
| lastIndexOf() | 查找指定元素的索引，返回最后一个符合元素的索引，不存在则返回-1 | 元素索引号或-1 |

### 数组转换为字符串

| 方法名         | 说明                                             | 返回值             |
| -------------- | ------------------------------------------------ | ------------------ |
| toString()     | 把数组转换为字符串，并用**逗号**分隔每个元素     | 返回转换后的字符串 |
| join(‘分隔符’) | 把数组转换为字符串，并用**指定的分隔符**每个元素 | 返回转换后的字符串 |

join方法如果不传入参数，则效果同toString

### 其它方法

| 方法名   | 说明                                      | 返回值                           |
| -------- | ----------------------------------------- | -------------------------------- |
| concat() | 连接两个或多个数组                        | 返回拼接后的数组，不影响原数组   |
| slice()  | 数组截取slice(begin, end)                 | 返回截取到的新数组，不影响原数组 |
| splice() | 数组删除splice(第一个开始， 要删除的个数) | 返回被删除后的数组，影响原数组   |

```js
var arr = [11, 22, 33, 33, 44, 55, 66];
var arrnew = ['qqq', '123', true];

//concat 测试
console.log(arr.concat(arrnew));
console.log(arr);
console.log(arrnew);

//slice 测试
console.log(arr.slice(2, 4));
console.log(arr);

//splice 测试
console.log(arr.splice(2, 4)); //[33, 33, 44, 55]
console.log(arr); //[11, 22, 66]
```

## 字符串对象

为了方便操作基本数据类型，js提供了三种特殊的引用类型：String、Number、Boolean

基本数据类型是简单数据类型，没有属性和方法，js利用以上三种引用类型，将其包装为复杂数据类型，使其有了某些属性和方法

```js
var str = 'andy';
console.log(str.length);//4
```

### 字符串的不可变性

```js
var str = '123abc';
str = '456qwe';
```

以上代码，看上去str变化了，其实是申请了新的内存空间，并赋了新的值，原先空间的值并不会发生变化

因此，在频繁进行字符串拼接操作时，由于单核CPU一次运算只能有两个操作数（组成原理），会产生大量的中间变量，反复的申请内存空间，非常耗时，效率非常低

### 根据字符返回索引

字符串变量经过包装后，可以调用部分方法来操作字符串

| 方法名                    | 说明                     | 返回值         |
| ------------------------- | ------------------------ | -------------- |
| index(‘字符’，开始的位置) | 从前往后找，匹配到即返回 | 返回索引或者-1 |
| lastIndexOf()             | 从后往前找，匹配到即返回 | 返回索引或者-1 |

```js
var str = '012345678901234567890';
console.log(str.indexOf('3', 3)); //3
console.log(str.lastIndexOf('4')); //14
```

### 根据索引返回字符

| 方法名            | 说明                          |
| ----------------- | ----------------------------- |
| charAt(index)     | 返回对应索引位置字符          |
| charCodeAt(index) | 返回对应索引位置字符的ASCII码 |
| str[index]        | 获取指定索引处的字符          |

```js
var str = '012345678901234567890';
console.log(str.charAt(3)); //3
console.log(str.charCodeAt(4)); //52
console.log(str[5]); //5
```

### 字符串操作方法

| 方法名                          | 说明                                  |
| ------------------------------- | ------------------------------------- |
| concat(str...)                  | 连接多个字符串，等效于`+`，原串不改变 |
| substr(start，length)           | 获取子串，原串不改变                  |
| slice(start，end)               | 截取字符串，原串不改变                |
| substring(start，end)           | 基本同slice，但是不接受负值           |
| replace(‘替换前的’，‘替换后的’) | 字符串替换，原串不改变                |
| split(“分割符”)                 | 将字符串分隔为数组                    |

```js
var str = '01234567';
var strnew = 'abcdefghjik';
//concat
console.log(str.concat(strnew));
console.log(str);
console.log(strnew);
//substr/slice/substring
console.log(str.substr(1, 5));
console.log(str.slice(1, 5));
console.log(str.substring(1, 5));
console.log(str);
//replace
console.log(str.replace('2', '0'));
console.log(str);
//split
var strnew = 'abcde ccc:123  777!qav';
console.log(strnew.split(':')); //["abcde ccc", "123  777!qav"]
console.log(strnew);
```

# 简单和复杂数据类型

## 简单数据类型

简单数据类型、基本数据类型、值类型：变量中存储的是值本身，包括String、Number、Boolean、undefined、null等

## 复杂数据类型

复杂数据类型、引用类型：变量中存储的是地址（引用），通过new关键字来创建对象，如Object、Array、Date

## 传参

简单类型进行值传递，所以函数的形参改变，不会影响到实参

复杂类型进行引用传递，所以函数可以改变对象内的属性

