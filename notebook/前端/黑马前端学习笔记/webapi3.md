# 元素的style属性

* 通过元素的style属性，可以**获取**和**设置**行内CSS样式
* 无法获取外部CSS文件中定义的样式
* 返回的值带单位

其中元素的宽度 style.width，不包含padding和border

# 元素的offset属性

* 使用offset系列属性，可以动态地得到元素的
  * 位置：与带有定位父元素的距离
  * 大小：元素本身的大小
* 可以获得任意样式表中的样式值
* offset属性是只读的，返回的数值不带单位

| 属性                 | 作用                                         |
| -------------------- | -------------------------------------------- |
| element.offsetParent | 返回该元素带有定位的父级元素，没有则返回body |
| element.offsetTop    | 返回距离带有定位父元素Top的偏移              |
| element.offsetLeft   | 返回距离带有定位父元素Left的偏移             |
| element.offsetWidth  | 返回自身包括padding、border、内容区的宽度    |
| element.offsetHeight | 返回自身包括padding、border、内容区的高度    |

# 元素的clent属性

* 使用client相关属性来获取元素可视区的相关信息
* 通过client相关属性可以动态得到元素的边框大小、元素大小等

| 属性                 | 作用                                                         |
| -------------------- | ------------------------------------------------------------ |
| element.clientTop    | 返回元素上边框的大小                                         |
| element.clientLeft   | 返回元素左边框的大小                                         |
| element.clientWidth  | 返回自身包括padding、内容区的宽度，不包含border，返回指不带单位 |
| element.clientHeight | 返回自身包括padding、内容区的高度，不包含border，返回指不带单位 |

# 元素的scroll属性

* 使用scroll相关属性可以动态地得到该元素的大小、滚动距离等
* 如果浏览器的高度不足时，会自动出现滚动条，当滚动条下滚时，上方被隐藏的高度，称为页面被卷去的头部，滚动条在滚动时，触发onscroll事件

| 属性                 | 作用                                         |
| -------------------- | -------------------------------------------- |
| element.scrollTop    | 返回被卷去的上侧距离，返回值不带单位         |
| element.scrollLeft   | 返回被卷去的左侧距离，返回值不带单位         |
| element.scrollWidth  | 返回自身实际的宽度，不含边框，返回值不带单位 |
| element.scrollHeight | 返回自身实际的高度，不含边框，返回值不带单位 |

## 兼容性解决方案

要注意的是，页面被卷去的头部，有兼容性问题，因此被卷去的头部通常有如下几种写法：

1. 声明了 DTD，使用 document.documentElement.scrollTop
2. 未声明 DTD，使用  document.body.scrollTop
3. 新方法 window.pageYOffset和 window.pageXOffset，IE9 开始支持

```javascript
function getScroll() {
    return {
      left: window.pageXOffset || document.documentElement.scrollLeft || document.body.scrollLeft||0,
      top: window.pageYOffset || document.documentElement.scrollTop || document.body.scrollTop || 0
    };
 } 
//使用的时候  getScroll().left

```

# 三大系列总结

| 三大系列宽度        | 作用                                                    |
| ------------------- | ------------------------------------------------------- |
| element.offsetWidth | 返回自身包括padding、border、内容区的宽度，不带单位     |
| element.clientWidth | 返回自身包括padding、内容区的宽度，不含border，不带单位 |
| element.scrollWidth | 返回自身宽度，不含border，不带单位                      |

* offset系列通常用于获取元素位置：offsetLeft、offsetTop
* client系列通常用于获取元素大小：clientWidth、clientHeight
* scroll系列通常用于获取滚动距离：scrollTop、scrollLeft
* 页面滚动的距离：windo.pageXOffset







# 触摸屏

## 触屏事件

相比PC端，移动端增加了触屏事件（touch event）

| 事件       | 说明                          |
| ---------- | ----------------------------- |
| touchstart | 手指触摸到一个DOM元素时触发   |
| touchmove  | 手指在一个DOM元素上滑动时触发 |
| touchend   | 手指从一个DOM元素上移开时触发 |

## 触屏事件对象

* TouchEvent 是一类描述手指在触屏上的状态变化的事件
* 这类事件用于描述一个或多个触点，使开发者可以检测触点的移动、增加、减少等
* touchstart、touchmove、touchend三个事件都有各自的事件对象

| 触摸列表       | 说明                                  |
| -------------- | ------------------------------------- |
| touches        | 正在触摸屏幕的所有手指的一个列表      |
| targetTouches  | 正在触摸当前DOM元素上的手指的一个列表 |
| changedTouches | 手指状态发生了改变的列表              |

## 移动端拖动元素

* touchstart、touchmove、touchend可以实现拖动元素
* 拖动元素需要当前手指的坐标值，我们可以用targetTouches[0].pageX 和 pageY
* 拖动原理：手指移动时，计算移动的距离，然后用盒子的位置+移动的距离

* 移动的距离：手指的当前位置，减去刚开始触摸的位置

### 步骤

1. 触摸元素touchstart：获取手指初始坐标，同时获得盒子原来的位置
2. 移动手指touchmove：计算手指滑动的距离，并且移动盒子
3. 手指离开touchend

注意：手指移动会触发屏幕滚动，因此要prevenDefault禁止默认行为