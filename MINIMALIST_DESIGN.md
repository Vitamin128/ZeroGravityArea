# 极简主义设计文档

## 🎨 设计配置

基于用户偏好定制的极简风格，温暖、优雅、书卷气。

### 色彩系统

```css
/* 背景色 */
--bg-primary: #FFFEF9;      /* 米白色主背景 */
--bg-secondary: #F7F4ED;    /* 浅米色次要背景 */
--bg-white: #FFFFFF;        /* 纯白卡片背景 */

/* 文字色 */
--text-primary: #2C2C2C;    /* 主要文字 */
--text-secondary: #6B6B6B;  /* 次要文字 */

/* 强调色 */
--accent: #8B6F47;          /* 深棕色 - 按钮、链接、活跃状态 */
--accent-hover: #6B5437;    /* 深棕色悬停态 */

/* 边框色 */
--border: #E8E3DA;          /* 1px 细线分隔 */

/* 滚动条 */
--scrollbar-track: #F7F4ED;
--scrollbar-thumb: #D4C4B0;
--scrollbar-hover: #8B6F47;

/* 状态色 */
--error: #D32F2F;
--success: #8B6F47;
```

### 字体系统

```css
/* 正文字体 - 优雅衬线 */
font-family: 'Lora', 'Crimson Text', 'Georgia', serif;

/* 标题/UI元素 - 现代无衬线 */
font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;

/* 字号 */
--font-base: 16px;
--font-small: 0.9rem;
--font-medium: 1rem;
--font-large: 1.1rem;
--font-title: 1.3rem;
```

### 圆角系统

```css
/* 微圆角 (2-4px) */
--radius-small: 2px;   /* 按钮、输入框 */
--radius-medium: 4px;  /* 卡片、模态框 */
```

### 阴影系统

```css
/* 轻微阴影 */
--shadow-sm: 0 1px 3px rgba(0, 0, 0, 0.05);
--shadow-md: 0 2px 8px rgba(0, 0, 0, 0.08);
--shadow-lg: 0 4px 16px rgba(0, 0, 0, 0.12);
```

### 间距系统

```css
--spacing-xs: 8px;
--spacing-sm: 12px;
--spacing-md: 16px;
--spacing-lg: 24px;
--spacing-xl: 40px;
--spacing-2xl: 60px;
```

## 📐 布局特点

### 侧边栏导航
- 顶部固定细条（60px 高）
- 汉堡菜单触发侧边栏
- 侧边栏宽度：280px
- 左侧滑入动画
- 遮罩层点击关闭

### 主内容区
- 左侧留出空间（margin-left: 80px）
- 内边距：40px 60px
- 响应式布局

## 🎭 交互设计

### 导航链接
- 悬停：背景色变化 + 左侧 3px 竖线
- 活跃：深棕色文字 + 浅米色背景 + 左侧竖线
- 过渡：0.2s ease

### 按钮
- 悬停：背景色变化 + 边框色变化
- 无位移动画
- 过渡：0.2s ease

### 模态框
- 遮罩：rgba(0, 0, 0, 0.4)
- 卡片：白色背景 + 细边框 + 轻微阴影
- 入场动画：淡入 + 向上位移

### Toast 通知
- 顶部居中（top: 80px）
- 白色背景 + 左侧彩色边框
- 图标 + 文字
- 3秒自动消失

## 📁 已更新文件

1. **src/App.vue**
   - 移除星空背景
   - 米白色背景
   - 全局字体设置
   - 滚动条样式

2. **src/components/NavBar.vue**
   - 顶部细条 + Logo
   - 汉堡菜单按钮
   - 侧边栏导航
   - 用户登录按钮

3. **src/components/auth/AuthModal.vue**
   - 极简白色卡片
   - 细边框设计
   - Toast 通知样式

## 🎯 设计原则

1. **克制** - 只用必要的视觉元素
2. **留白** - 充足的空间让内容呼吸
3. **层次** - 通过字重和色彩区分层级
4. **一致** - 统一的圆角、间距、阴影
5. **优雅** - 衬线字体营造书卷气质

## 🔄 后续建议

### 需要更新的组件
- LoginForm.vue
- RegisterForm.vue
- ForgotForm.vue
- SettingsForm.vue
- 所有视图页面（Gaming.vue, MainPage.vue 等）

### 建议的表单样式
```css
input, textarea {
  border: 1px solid #E8E3DA;
  border-radius: 4px;
  padding: 12px 16px;
  font-size: 1rem;
  transition: border-color 0.2s ease;
}

input:focus, textarea:focus {
  outline: none;
  border-color: #8B6F47;
}

button.primary {
  background: #8B6F47;
  color: white;
  border: none;
  padding: 12px 24px;
  border-radius: 4px;
  font-weight: 500;
}

button.primary:hover {
  background: #6B5437;
}
```

### 建议的卡片样式
```css
.card {
  background: #FFFFFF;
  border: 1px solid #E8E3DA;
  border-radius: 4px;
  padding: 24px;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
}
```

## 📱 响应式建议

```css
@media (max-width: 768px) {
  .main-content {
    margin-left: 0;
    padding: 20px;
  }
  
  .sidebar {
    width: 100%;
  }
}
```

---

**设计完成时间**: 2026/05/19  
**风格**: 极简主义 - 米白暖色系  
**特点**: 温暖、优雅、书卷气
