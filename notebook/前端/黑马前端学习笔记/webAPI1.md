# web API

Web API 是浏览器提供的一套操作浏览器功能和页面元素的 API ( BOM 和 DOM )

# DOM

* 文档对象模型（Document Object Model）：是W3C组织推荐的处理可拓展标记语言的标准编程接口，所有的浏览器都遵循这套规范
* 通过DOM接口，可以改变网页的内容、结构、样式

## DOM树

DOM树又称为文档树模型，把文档映射成树形结构，通过节点对象对其处理，处理的结果可以加入到当前页面

* 文档：一个页面就是一个文档，DOM中使用document表示
* 节点：网页中的所有内容，在文档树中都是节点（标签、属性、文本、注释等），使用node表示
* 标签节点：网页中的所有标签，通常称为元素节点，又简称为“元素”，使用element表示

## 获取元素

想要操作文档中的内容，必须先获取节点对象

### 根据ID获取

document.getElementById

返回元素对象或者null

```js
<div id="num">123</div>
<script>
    var num = document.getElementById('num');
    console.log(num);			//<div id="num">123</div>
    console.log(typeof num);	//object
    console.log(num.innerHTML);	//123
</script>
```

### 根据标签名获取

document.getElementsByTagName

element.getElementsByTagName

根据标签名获取元素对象，返回元素对象集合（伪数组），这个集合是动态的，当页面中增加对应的元素，该集合也会增加

```js
<div>123</div>
<div>456</div>
<div>789</div>
<script>
    var num = document.getElementsByTagName('div');
    console.log(num);				//HTMLCollection(3) [div, div, div]
    console.log(typeof num);		//object
    for (var i = 0; i < num.length; i++)
            console.log(num[i].innerHTML);	//123，456，789
</script>
```

### 根据H5新方法获取

* getElementsByClassName：通过类名选择，返回元素对象集合
* querySelector：通过选择器选择，返回符合条件的第一个元素对象
* querySelectorAll：通过选择器选择，返回元素对象集合

**注意：通过选择器选择时，参数与css中的选择器一致，要加符号**

```js
<div class="num1">123</div>
<span class="num1">qqq</span>
<div class="num2">456</div>
<p id="ptest">abcd</p>
<script>
    var num1 = document.getElementsByClassName('num1');
    console.log(num1);	//HTMLCollection(2) [div.num1, span.num1]
    var num2 = document.querySelector('div');
    console.log(num2);	//<div class="num1">123</div>
    var num2 = document.querySelector('.num2');
    console.log(num2);	//<div class="num2">456</div>
    var num = document.querySelectorAll('div');
    console.log(num);	//NodeList(2) [div.num1, div.num2]
	var ppp = document.querySelector('#ptest');
    console.log(ppp);	//<p id="ptest">abcd</p>
</script>
```

### 获取特殊元素

获取body或html元素，注意，它们不是方法，不需要加括号

* document.body
* document.html

## 事件基础

### 概述

* 事件：可以被js侦测到的行为，例如按钮按下，鼠标按下，鼠标移动等
* 网页中的每个元素，都可以产生事件，因此可以在js代码中，对事件做出响应

### 事件三要素

* 事件源：是谁触发的事件
* 事件类型：什么类型的事件，例如点击click
* 事件处理程序：事件发生后，执行什么处理函数

### 执行事件的步骤

1. 获取事件源
2. 注册事件（绑定事件）
3. 添加事件处理程序（函数赋值）

```js
<button id="btn">按钮</button>
<script>
    //事件源btn
    var btn = document.getElementById('btn');
	//事件类型按下，事件处理程序
	btn.onclick = function() {
    	alert("按钮被按下了");
	};
</script>
```

### 常见的鼠标事件

| 鼠标事件    | 触发条件     |
| ----------- | ------------ |
| onclick     | 鼠标左键点击 |
| onmouseover | 鼠标经过     |
| onmouseout  | 鼠标离开     |
| onfocus     | 获得焦点     |
| onblur      | 失去焦点     |
| onmousemove | 鼠标移动     |
| onmousedown | 鼠标按下     |

## 操作元素

JavaScript的 DOM 操作可以改变网页**内容**、**结构**和**样式**，我们可以利用 DOM 操作元素来改变元素里面的内容、属性等。（注意：这些操作都是通过元素对象的属性实现的）

### 获取或改变元素内容

```js
element.innerText = ‘文本’;
element.innerHTML = ‘文本’;
```

* 区别：
  * innerText有兼容性问题，推荐使用innerHTML
  * innerHTML包含html标签，innerText不包含html标签
  * 首尾的空格会被去掉，中间如果有多个空格，只保留一个

### 元素属性操作

* element.属性名：获取属性
* element.属性名= ...：设置属性

```js
<a href="http://www.mi.com" id="link">hhhh</a>
<script>
    var link = document.getElementById('link');
	console.log(link.href);
	link.href = 'http://www.taobao.com';
</script>
```

### 样式属性操作

可以通过JS修改元素的样式

* 方式1：修改行内样式 **element.style**
  * 原来在css中用`-`连接的样式名，在js中变为驼峰命名法，如fontSize
  * 行内样式的权重较高

```js
<a href="#" id="link">hhhh</a>
<script>
    var link = document.getElementById('link');
	link.style = 'display:block; width:100px; height:100px;';
</script>
```

* 方式2：修改类名 **element.className**
  * 如果需要修改的样式较多，建议该方法
  * class是保留字，因此使用className
  * 该方法会覆盖原先的类名

```js
<style>
    .style1 {
        display: block;
        width: 100px;
        height: 100px;
        background-color: red;
    }

    .style2 {
        display: block;
        width: 100px;
        height: 100px;
        background-color: blue;
    }
</style>

<a href="#" id="link" class="style1">hhhh</a>
<script>
    var link = document.getElementById('link');
    link.className = 'style2';
</script>
```

### 自定义属性操作

自定义属性必须使用以下三个API操作，不能像内置属性那样操作

* setAttribute：设置自定义属性
* getAttribute：获取自定义属性
* removeAttribute：移除自定义属性

```js
<button class="normal" id="qqq">按钮1</button>

<script>
    var btn = document.getElementById('qqq');
    btn.setAttribute('index', 10);
    console.log(btn.getAttribute('index')); //10
    btn.removeAttribute('index');
    console.log(btn.getAttribute('index')); //null
</script>
```

#### H5自定义属性

自定义属性很容易引起歧义，不容易判断到底是自定义的还是内置的，因此H5增加了自定义属性

H5规定自定义属性以`data-`开头，例如

```html
<div data-index='1'></div>
```

`dataset`是一个以`data-`开头的自定义属性的集合

* 设置自定义属性

```js
//老方法
element.setAttribute('data-index', 10);
//h5新方法
element.dataset.index = 10;
element.dataset['index'] = 10;
```

* 获取自定义属性

```js
//老方法
element.getAttribute('data-index');
//h5新方法
console.log(btn.dataset.index);
console.log(btn.dataset['index']);
```

# 节点操作

## 概述

* 网页中的所有内容都是节点（标签、属性、文本、注释等），在DOM 中，节点使用 node 来表示
* HTML DOM 树中的所有节点均可通过 JavaScript 进行访问，所有 HTML 元素（节点）均可被修改，也可以创建或删除
* 一般节点至少拥有nodeType（节点类型）、nodeName（节点名称）和nodeValue（节点值）这三个基本属性，实际开发中，主要操作元素节点
  * nodeType = 1：元素节点
  * nodeType = 2：属性节点
  * nodeType = 3：文本节点
* 利用DOM树，可以把节点划分未不同的层级关系（父子兄）

## 父节点

node.parentNode 返回该节点最近的父节点，如果没有父节点则返回null

```js
<div>
    <button class="normal" id="qqq">按钮1</button>
</div>

<script>
    var btn = document.getElementById('qqq');
	console.log(btn.parentNode);
</script>
```

## 子节点

* father.childNodes：所有的子节点，包括属性节点、文本节点
  * 该集合是实时更新的
  * 包含了所有的子节点，包括元素节点、属性节点、文本节点
  * 如果只想获取里面的元素节点，需要专门处理，一般不推荐使用

```js
<div id="father">
    <button class="normal" id="son">按钮1</button>
    <button class="normal">按钮2</button>
    <button class="normal">按钮3</button>
    <button class="normal">按钮4</button>
</div>

<script>
    var father = document.getElementById('father');
    console.log(father.childNodes);
</script>
```

* father.children：返回所有子元素节点，注意只返回元素节点，该属性只读

```js
console.log(father.children);
```

### 第N个节点

同childNodes一样，以下两种方式也是返回所有的节点，找不到则返回null

这两种方法不推荐使用，因为会返回所有节点，不便操作

* 第一个节点：father.firstChild
* 最后一个节点：father.lastChild

<hr>

同children一样，以下两种方式只返回元素节点

这两种方法有兼容性问题，也不推荐使用

* 第一个子元素节点：father.firstElementChild
* 最后一个子元素节点：father.lastElementChild

<hr>

实际使用中的解决方案

* 第一个子元素节点：father.children[0]
* 最后一个子元素节点：fater.children[fater.children.length - 1]

### 兄弟节点

xxxSibling返回的节点，包括文本节点、注释节点、换行、空格、文本等

* 下一个兄弟节点：node.nextSibling
* 上一个兄弟节点：node.previousSibling

xxxElementSibling返回的节点，是同等层级下的节点，不包括下一层的诸如文本节点、注释节点，但是有兼容性问题

* 下一个兄弟节点：node.nextElementSibling
* 上一个兄弟节点：node.previousElementSibling

**如何解决兼容性问题**

调用node.nextSibling，循环去获取节点，如果获取到的nodeType = 1时，才返回，这样能实现nextElementSibling的效果

```js
function getNextElementSibling(element) {
    var el = element;
    while (el = el.nextSibling) {
        if (el.nodeType === 1) {
            return el;
        }
    }
    return null;
}  
```

## 创建节点

document.createElement()方法创建指定标签的HTML元素，称为动态创建元素

## 添加节点

* node.appendChild将元素添加到节点末尾
* node.insertBefore将元素插入到指定的子节点之前

```js
<div id="father">
    <button class="normal" id="son">1</button>
    <button class="normal">2</button>
</div>

<script>
    var father = document.getElementById('father');
    var btn1 = document.createElement('button');
    btn1.className = 'normal';
    btn1.innerHTML = '3';
    father.appendChild(btn1);

    var btn2 = document.createElement('button');
    btn2.className = 'normal';
    btn2.innerHTML = '4';
    father.insertBefore(btn2, father.children[0]);
</script>
```

## 删除节点

node.removeChild(子节点)：删除对应的子节点，并返回被删除的节点

```js
console.log(father.removeChild(father.children[0]));
```

## 复制节点

* node.cloneNode()：浅拷贝，只复制节点本身，不复制其中的子节点（文本节点、属性节点之类的）
* node.cloneNode(true)：深拷贝，完全复制节点，包括所有的子节点

```js
var father = document.getElementById('father');
var btn1 = father.children[0].cloneNode();
var btn2 = father.children[0].cloneNode(true);
father.appendChild(btn1);
father.appendChild(btn2);
```

## 创建元素的三种方式

* document.write()
* element.innerHTML()
* document.createElement()

```js
<div id="father">
    <div id="son">1</div>
</div>

<script>
    var node = document.getElementById('son');
	node.onclick = function() {
    	//1. 点击div时，调用write会导致整个页面重绘
        document.write('<div>123</div>');
        //2. 点击div时，在div内部增加超链接文本
        node.innerHTML += '<a href="#">百度</a>';
        //2. 利用数组拼接，一次性增加大量节点
        var arr = [];
        for (var i = 0; i <= 100; i++) {
            arr.push('<a href="#">百度</a>');
        }
        node.innerHTML = arr.join('');
        //3. createElement创建节点后添加
        for (var i = 0; i <= 100; i++) {
            var a = document.createElement('a');
            a.innerHTML = "baidu";
            node.appendChild(a);
        }  
	};
</script>
```

### 三种方式的对比

* document.write：会导致重绘，不推荐使用
* node.innerHTML拼接字符串：效率极低，反复申请内存
* node.innerHTML拼接数组：效率高，结构复杂
* document.createElement：效率一般，容易理解