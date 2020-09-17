# 事件

## 注册事件

给元素添加事件，称为注册事件或者绑定事件

注册事件有两种方式：传统方式和监听注册方式

### 传统注册方式

* 利用on开头的事件注册
* 注册事件的唯一性：同一元素的同一事件，只能有一个处理函数

```js
<button id="btn">按钮</button>

<script>
    var btn = document.getElementById('btn');
    btn.onclick = function() {
        alert('按下了');
	}
</script>
```

### 监听注册方式

* IE9以后支持
* 事件类型 type：比如click、mouseove，注意不需要带on
* 事件处理函数 listener：事件发生时，调用该函数
* 可选参数useCapture：默认false

```js
//eventTarget.addEventListener(type, listener[, useCapture])
<button id="btn">按钮</button>

<script>
    var btn = document.getElementById('btn');
    btn.addEventListener('click', function() {
        alert('按下了');
    })
</script>
```

* IE6 7 8 支持
* 事件类型 eventNameWithOn：比如onclick、onmouseove
* 事件处理函数 callback：事件发生时，调用该函数

```js
//eventTarget.attachEvent(eventNameWithOn, callback)
<button id="btn">按钮</button>

<script>
    var btn = document.getElementById('btn');
    btn.attachEvent('onclick', function() {
        alert('按下了');
    })
</script>
```

## 事件监听兼容性解决办法
封装一个函数，先对浏览器版本进行判断

```js
function addEventListener(element, eventName, fn) {
    //判断浏览器是否支持addEventListener
    if (element.addEventListener) {
        element.addEventListener(eventName, fn);
    } else if (element.attachEvent) {
        element.attachEvent('on' + eventName, fn);
    } else {
        element['on' + eventName] = fn;
    }
}
```

## 删除事件(解绑)

### 传统注册事件的解绑

将之前注册的事件，赋值为空

```js
eventTargrt.onclick = null;
```

### 监听注册事件的解绑

```js
// 如果需要解绑，注册和解绑都要使用有名函数
eventTargrt.removeEventListener('click', fn);	//ie9 up
eventTargrt.detachEvent('onclick', fn);			//ie 6-7-8
```

### 删除事件兼容性解决方案

```js
function removeEventListener(element, eventName, fn) {
    if (element.removeEventListener) {
        element.removeEventListener(eventName, fn);
    } else if (element.detachEvent) {
        element.detachEvent('on' + eventName, fn);
    } else
        element['on' + eventName] = null;
}
```

# DOM事件流

* 事件流：描述从页面中接收事件的顺序
* DOM事件流：事件发生时，会在元素节点之间按照特定的顺序传播，这个传播过程叫DOM事件流（比如点击了一个DIV，也就点击了BODY）

* 事件冒泡：IE最早提出，事件开始时由具体的元素接收，然后逐级向上传播到DOM最顶层节点的过程
* 事件捕获：网景最早提出，由DOM最顶层节点开始，然后逐级向下传播到具体元素接收的过程
* DOM事件流会经历3个阶段
  * 捕获阶段
  * 当前目标阶段
  * 冒泡阶段
* JS代码中只能执行捕获或者冒泡其中的一个阶段
* onclick和attachEvent只能得到冒泡阶段
* addEventListener第三个参数如果是**true**，表示在捕获阶段调用事件处理程序；如果是false，表示在冒泡阶段调用事件处理程序
* 实际开发中很少使用事件捕获，更专注的是事件冒泡
* 有些事件没有冒泡，如onblur、onfocus、onmouseenter、onmouseleave

## 事件冒泡

以下代码，按键时打印顺序为`btn->div->doc`

```js
<div id="div">
    <button id="btn">按钮</button>
</div>

<script>
    var btn = document.getElementById('btn');
    var div = document.getElementById('div');

    btn.addEventListener('click', function() {
        alert('btn 按下');
    }, false);

    div.addEventListener('click', function() {
        alert('div 按下');
    }, false);

    document.addEventListener('click', function() {
        alert('doc 按下');
    }, false);
</script>
```

## 事件捕获

以下代码，按键时打印顺序为`doc->div->btn`

```js
<div id="div">
    <button id="btn">按钮</button>
</div>

<script>
    var btn = document.getElementById('btn');
    var div = document.getElementById('div');

    btn.addEventListener('click', function() {
        alert('btn 按下');
    }, true);

    div.addEventListener('click', function() {
        alert('div 按下');
    }, true);

    document.addEventListener('click', function() {
        alert('doc 按下');
    }, true);
</script>
```

## 事件对象
* 概念：事件发生后，跟事件相关的一系列信息数据的集合都放到这个对象里面，这个对象就是事件对象，例如
  * 谁绑定了这个事件
  * 鼠标触发事件，得到鼠标的相关信息
  * 键盘触发事件，得到键盘的相关信息

### 事件对象的使用

* 事件被触发时，就会产生事件对象
* 事件对象会以实参的形式传递给事件处理函数
* 一般在事件处理函数中，声明一个形参来接收事件对象

```js
var div = document.getElementById('div');

div.addEventListener('click', function(e) {
    console.log('鼠标点击');
    console.log('x=' + e.screenX + ', y=' + e.screenY);
}, true);
```

### 事件对象兼容性的处理

* 一般情况下，只需要定义形参e接收事件对象即可
* 在IE 6-7-8 中，浏览器不会给方法传递参数，如果需要的话，使用window.event获取

```js
e = e || window.event
```

### 事件对象的属性和方法

| 事件对象属性和方法  | 说明                   | 备注                |
| ------------------- | ---------------------- | ------------------- |
| e.target            | 返回触发事件的对象     |                     |
| e.srcElement        | 返回触发事件的对象     |                     |
| e.type              | 返回事件的类型，不带on | click、mouseover等  |
| e.cancelBubble      | 阻止冒泡               | 非标准，ie6-7-8使用 |
| e.returnValue       | 阻止默认事件           | 非标准，ie6-7-8使用 |
| e.preventDefault()  | 阻止默认行为           | 标准                |
| e.stopPropagation() | 阻止冒泡               | 标准                |

#### e.target和this的区别

* this是事件绑定的元素，绑定了事件处理函数
* e.target是事件触发的元素
* 通常情况下，两者是一致的
* 在**事件冒泡**时，点击子元素，父元素的事件处理函数也会执行，此时在父元素的事件处理函数中
  * this指向的是父元素
  * e.target指向的是子元素

## 阻止默认行为

* html中某些标签有默认行为，例如a点击后，页面自动跳转

```js
<a href="http://www.baidu.com" id="link">123</a>

<script>
    var link = document.getElementById('link');
    link.addEventListener('click', function(e) {
        //阻止自动跳转到指定网页
		e.preventDefault();
});
</script>
```

## 阻止事件冒泡

* 某个元素已经处理了事件，该事件就不再传到上层

```js
<div id="div">
    <button id="btn">按钮</button>
</div>


<script>
    var btn = document.getElementById('btn');
	var div = document.getElementById('div');

    btn.addEventListener('click', function(e) {
        alert('btn 按下');
        e.stopPropagation();
	}, false);

    div.addEventListener('click', function(e) {
        alert('div 按下');
    }, false);

    document.addEventListener('click', function(e) {
        alert('doc 按下');
    }, false);
</script>
```

## 事件委托

* 也称为事件代理，将事件交给其它元素去处理，在jQuery中称为事件委派

* 给父元素注册事件处理函数，当子元素的事件触发后，会冒泡到父元素，在父元素的事件处理函数中，判断具体是哪个子元素被触发了，然后做对应处理

* 只需要操作一次DOM，精简代码，提高了程序性能
* 能处理动态创建的子元素触发的事件

在以下例程中，每按下一个按键，它的背景色就发生变化，但是是在父元素中统一处理

```js
<div id="div">
    <button id="btn">按钮1</button>
    <button id="btn">按钮2</button>
    <button id="btn">按钮3</button>
</div>

<script>
    var div = document.getElementById('div');
    div.addEventListener('click', function(e) {
        e.target.style.backgroundColor = 'pink';
    });
</script>
```

# 常用的鼠标事件

| 鼠标事件  | 触发条件     |
| --------- | ------------ |
| click     | 鼠标点击左键 |
| mouseover | 鼠标经过     |
| mouseout  | 鼠标离开     |
| focus     | 获得鼠标焦点 |
| blur      | 失去鼠标焦点 |
| mousemove | 鼠标移动     |
| mouseup   | 鼠标弹起     |
| mousedown | 鼠标按下     |

## 禁止右键菜单

contextmenu是元素的一个属性，禁用掉了DOM的这个属性，所有元素在鼠标右键时均不会显示菜单

```js
document.addEventListener('contextmenu', function(e) {
    e.preventDefault();
})
```

## 禁止鼠标选中

selectstart表示鼠标开始选中

```js
document.addEventListener('selectstart', function(e) {
    e.preventDefault();
})
```

## 鼠标事件对象

event事件对象是事件相关的信息集合，其中常用的与鼠标相关的有

| 鼠标事件对象 | 说明                                  | 备注           |
| ------------ | ------------------------------------- | -------------- |
| e.clientX    | 返回鼠标相对于浏览器窗口可视区的X坐标 | 不随滚动条变化 |
| e.clientY    | 返回鼠标相对于浏览器窗口可视区的Y左边 | 不随滚动条变化 |
| e.pageX      | 返回鼠标相对于文档页面的X坐标，IE9+   | 跟随滚动条变化 |
| e.pageY      | 返回鼠标相对于文档页面的Y坐标，IE9+   | 跟随滚动条变化 |
| e.screenX    | 返回鼠标相对于电脑屏幕的X坐标         | 不随滚动条变化 |
| e.screenY    | 返回鼠标相对于电脑屏幕的Y坐标         | 不随滚动条变化 |

# 常用的键盘事件

## 键盘事件

| 键盘事件 | 触发条件                                                     |
| -------- | ------------------------------------------------------------ |
| keyup    | 松开时触发                                                   |
| keydown  | 按下时触发，长按时会一直触发                                 |
| keypress | 被按下时触发，长按时会一直触发，但是不识别功能键，如ctrl、alt、shift、箭头等 |

```js
document.addEventListener('keydown', function(e) {
    console.log('keydown ' + e.keyCode);
})
document.addEventListener('keyup', function(e) {
    console.log('keyup ' + e.keyCode);
})
document.addEventListener('keypress', function(e) {
    console.log('keypress ' + e.keyCode);
})
```

## 键盘事件对象

* keyCode：返回该键的键值

# BOM

## 什么是BOM

* BOM：browser object model，浏览器对象模型，它提供了独立于内容而与浏览器窗口进行交互的对象，其核心对象是window
* BOM由一系列相关的对象构成，并且每个对象都提供了很多方法与属性
* BOM缺乏标准，JavaScript的语法是标准化组织ECMA制定的，DOM是标准化是W3C制定的，而BOM最初只是Netscape浏览器标准的一部分

### DOM与BOM的区别

DOM

* 文档对象模型
* DOM就是把文档当作一个对象
* DOM的顶级对象是document
* DOM主要学习的是操作页面元素
* DOM是W3C标准规范

BOM

* 浏览器对象模型
* 把浏览器当作一个对象来看待
* BOM的顶级对象是window
* BOM学习的是浏览器窗口交互的一些对象
* BOM是浏览器厂商在各自浏览器上定义的，兼容性差

## BOM的构成

BOM比DOM更大，它包含DOM

* windows
  * document
  * location
  * navigation
  * screen
  * history

## window对象

* window对象是浏览器的顶级对象
* 它是JS访问浏览器窗口的接口
* 它是全局对象，定义在全局作用域的变量、函数，都会变成window对象的属性和方法

## window对象的常见事件

### 页面（窗口）加载事件

* load是页面（窗口）加载事件，当文档内容完全加载完成（包括图像、脚本文件、CSS文件等），会触发该事件，并调用注册的事件处理函数

* DOMContentLoaded 事件，仅当DOM加载完成时触发，不包括样式表、图片、flash等，IE9以上支持

```js
//老的方法 onload 只支持一个处理函数
window.onload = function() {
    alert('load');
}
//新方法addEventListener，对于load可以有多个处理函数
window.addEventListener('load', function() {
    alert('add load');
})
//DOM最先加载完成，比onload先调用
window.addEventListener('DOMContentLoaded', function() {
    alert('DOM load');
})
```

### 调整窗口大小事件

* resize 是调整窗口大小加载事件，当窗口大小改变时，触发该事件
* 该事件经常用来完成响应式布局
* window.innerWidth 属性是网页的宽度

```js
window.onresize = function() {
    console.log('onresize ' + window.innerWidth);
}
window.addEventListener('resize', function() {
    console.log('add resize ' + window.innerWidth);
})
```

## 定时器

### 炸弹定时器 setTimeout

* 使用示例

```js
//创建一个定时器
var timer1 = window.setTimeout(function() {
    console.log('午时已到');
}, 1000);
//删除定时器
window.clearTimeout(timer1);
```

* 第一个参数是回调函数，第二个参数是经过多少毫秒后触发，可以省略，省略时默认为0，立即触发

* 需要多个定时器时，可以用变量接收返回值
* 不需要使用时可以删除定时器

### 闹钟定时器 setInterval

* 使用示例

```js
//开启定时器
var timer1 = window.setInterval(function() {
    console.log('午时已到');
}, 1000)
//停止定时器
window.clearInterval(timer1);
```

* 使用上基本同炸弹定时器，但是定时时长参数省略时，不会触发

## location 对象

* window提供了一个location属性，用于获取和设置URL，并且可以用于解析URL

* location属性返回一个对象，因此将这个属性也称为location对象

### URL

* 统一资源定位符（uniform resource locator，URL）是互联网上标准资源的地址
* 互联网上每个文件都有一个唯一的URL，它包含的信息指出文件的位置以及浏览器应该怎样处理它
* 一般的语法格式为

```http
protocal://host[:port]/path/[?query]#fragment
http://www.baidu.com/index.html?name=andy&age=18#link
```

| 组成     | 说明                               |
| -------- | ---------------------------------- |
| protocol | 通信协议，常用的http/ftp/maito等   |
| host     | 主机（域名）www.baidu.com          |
| port     | 端口号（可选），默认http端口号为80 |
| path     | 表示主机上一个文件或目录的地址     |
| query    | 参数以键值对的形式通过&符号隔开    |
| fragment | 片段，#后面的内容，常见于链接锚点  |

### location对象的属性

| 属性              | 返回值                       |
| ----------------- | ---------------------------- |
| location.href     | 获取或者设置URL              |
| location.host     | 返回主机名（域名）           |
| location.port     | 返回端口号，如果没写则返回空 |
| location.pathname | 返回路径                     |
| location.search   | 返回参数                     |
| location.hash     | 返回片段                     |

### location对象的常见方法

| location           | 说明                                       |
| ------------------ | ------------------------------------------ |
| location.assign()  | 与href一样，可以跳转页面                   |
| location.replace() | 替换当前页面，但是由于不记录历史，不能后退 |
| location.reload()  | 重新加载页面，效果同F5                     |

## navigator对象

navigator对象包含有关浏览器的信息，它有很多属性，我们最常用的是userAgent，该属性可以返回由客户机发送服务器的user-agent头部的值

下面前端代码可以判断用户那个终端打开页面，实现跳转

```js
if((navigator.userAgent.match(/(phone|pad|pod|iPhone|iPod|ios|iPad|Android|Mobile|BlackBerry|IEMobile|MQQBrowser|JUC|Fennec|wOSBrowser|BrowserNG|WebOS|Symbian|Windows Phone)/i))) {
    window.location.href = "";     //手机
 } else {
    window.location.href = "";     //电脑
 }
```

## history对象

window对象给我们提供了一个 history对象，与浏览器历史记录进行交互。该对象包含用户（在浏览器窗口中）访问过的URL，history对象一般在实际开发中比较少用，但是会在一些 OA 办公系统中见到。

| 方法      | 说明             |
| --------- | ---------------- |
| back()    | 后退             |
| forward() | 前进             |
| go(参数)  | 后退/前进N个界面 |















