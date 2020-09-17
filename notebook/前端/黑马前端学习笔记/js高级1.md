# 类

* 在ES6中新增加了类的概念，可以使用class关键字声明一个类，类抽象了对象的公共部分，用这个类来实例化对象

## 类的基本使用方法

```js
class Star {
    //构造函数初始化公共属性
    constructor(name, age) {
        this.name = name;
        this.age = age;
    }
    //方法
    sing(song) {
        console.log(this.name + '会唱' + song);
    }
}
//对象实例化
var ldh = new Star('刘德华', 18);
console.log(ldh.name + ldh.age);
//调用对象方法
ldh.sing('沙漠骆驼');
```

## 类的继承

* 在构造函数中，子类使用super访问父类的方法，完成公有属性的初始化，然后在做子类特有的属性初始化
* 子类实例化对象后，调用方法时先找子类有没有该方法，没有才去父类找
* 子类可以重写父类的方法
* 在ES6中没有类的提升，因此class要先定义，后使用

```js
class Father {
    constructor(name) {
        this.name = name;
    }
    speak(lang) {
        console.log(this.name + '会说' + lang);
    }
}
class Son extends Father {
    constructor(name, age) {
//利用super调用父类的构造函数
        super(name);
//子类特有的属性初始化
        this.age = age;
    }
//重写父类方法
    speak(lang) {
        console.log('我对钱没有兴趣');
    }
}
var jack = new Son('马云', 50);
console.log(jack.name);
console.log(jack.age);
jack.speak('英语');
```

## 静态成员和实例成员

* 实例成员：在构造函数内部通过this添加的成员，只能通过实例化的对象来访问
* 静态成员：在构造函数本身添加的成员，只能通过构造函数来访问

## 构造函数原型

### 构造函数的问题

构造函数实例化多个对象的时候，每个对象的方法，都会单独申请内存空间，这和c++是不一样的，因此比较消耗内存，所以希望做到所有对象共享方法

## 构造函数原型prototype

构造函数通过原型分配的函数是所有对象所共享的。

JavaScript 规定，每一个构造函数都有一个prototype 属性，指向另一个对象。注意这个prototype就是一个对象，这个对象的所有属性和方法，都会被构造函数所拥有。

我们可以把那些不变的方法，直接定义在 prototype 对象上，这样所有对象的实例就可以共享这些方法。

```js
function Star(uname, age) {
    this.uname = uname;
    this.age = age;
}
Star.prototype.sing = function() {
	console.log('我会唱歌');
}
var ldh = new Star('刘德华', 18);
var zxy = new Star('张学友', 19);
ldh.sing();//我会唱歌
zxy.sing();//我会唱歌
```

## 对象原型

对象都会有一个属性 __proto__ 指向构造函数的 prototype 原型对象，之所以我们对象可以使用构造函数 prototype 原型对象的属性和方法，就是因为对象有 __proto__ 原型的存在。
__proto__对象原型和原型对象 prototype 是等价的

__proto__对象原型的意义就在于为对象的查找机制提供一个方向，或者说一条路线，但是它是一个非标准属性，因此实际开发中，不可以使用这个属性，它只是内部指向原型对象 prototype

## constructor构造函数

