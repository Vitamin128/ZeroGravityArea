<template>
  <div class="auth-modal-overlay" @click.self="$emit('close')">
    <div class="auth-card-wrapper">
      <!-- 关闭按钮 -->
      <button class="close-btn" @click="$emit('close')">
        <Icon icon="ph:x-bold" />
      </button>

      <transition name="fade-slide" mode="out-in">
        <!-- 登录视图 -->
        <div v-if="currentView === 'login'" class="auth-view" key="login">
          <h2>欢迎回来</h2>
          <p class="subtitle">登录您的 Zero Gravity Area 账号</p>
          
          <div class="input-group">
            <Icon icon="ph:user-bold" class="input-icon" />
            <input type="text" placeholder="用户名 / 邮箱" />
          </div>
          
          <div class="input-group">
            <Icon icon="ph:lock-key-bold" class="input-icon" />
            <input type="password" placeholder="密码" />
          </div>
          
          <div class="actions-row">
            <label class="checkbox-container">
              <input type="checkbox" />
              <span class="checkmark"></span>
              记住我
            </label>
            <a href="#" class="text-link" @click.prevent="switchView('forgot')">忘记密码？</a>
          </div>
          
          <button class="submit-btn">登 录</button>
          
          <p class="switch-hint">
            还没有账号？ <a href="#" class="text-link accent" @click.prevent="switchView('register')">立即注册</a>
          </p>
        </div>

        <!-- 注册视图 -->
        <div v-else-if="currentView === 'register'" class="auth-view" key="register">
          <h2>创建账号</h2>
          <p class="subtitle">加入 Zero Gravity Area</p>
          
          <div class="input-group">
            <Icon icon="ph:user-bold" class="input-icon" />
            <input type="text" placeholder="用户名" />
          </div>
          
          <div class="input-group">
            <Icon icon="ph:envelope-simple-bold" class="input-icon" />
            <input type="email" placeholder="邮箱地址" />
          </div>
          
          <div class="input-group">
            <Icon icon="ph:lock-key-bold" class="input-icon" />
            <input type="password" placeholder="密码" />
          </div>

          <div class="input-group">
            <Icon icon="ph:lock-key-bold" class="input-icon" />
            <input type="password" placeholder="确认密码" />
          </div>
          
          <button class="submit-btn">注 册</button>
          
          <p class="switch-hint">
            已有账号？ <a href="#" class="text-link accent" @click.prevent="switchView('login')">返回登录</a>
          </p>
        </div>

        <!-- 找回密码视图 -->
        <div v-else-if="currentView === 'forgot'" class="auth-view" key="forgot">
          <h2>找回密码</h2>
          <p class="subtitle">输入您的邮箱以重置密码</p>
          
          <div class="input-group">
            <Icon icon="ph:envelope-simple-bold" class="input-icon" />
            <input type="email" placeholder="注册邮箱地址" />
          </div>
          
          <button class="submit-btn">发送重置链接</button>
          
          <p class="switch-hint">
            记起密码了？ <a href="#" class="text-link accent" @click.prevent="switchView('login')">返回登录</a>
          </p>
        </div>
      </transition>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue';
import { Icon } from '@iconify/vue';

const emit = defineEmits(['close']);
const currentView = ref('login'); // 'login', 'register', 'forgot'

const switchView = (view) => {
  currentView.value = view;
};
</script>

<style scoped>
.auth-modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
  background: rgba(0, 0, 0, 0.6);
  backdrop-filter: blur(8px);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 2000; /* 高于 Navbar */
}

.auth-card-wrapper {
  position: relative;
  width: 400px;
  background: rgba(30, 30, 30, 0.7);
  backdrop-filter: blur(20px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 24px;
  padding: 40px 30px;
  box-shadow: 0 20px 50px rgba(0, 0, 0, 0.5);
  color: white;
  overflow: hidden;
  transition: height 0.4s cubic-bezier(0.25, 0.8, 0.25, 1);
}

.close-btn {
  position: absolute;
  top: 20px;
  right: 20px;
  background: none;
  border: none;
  color: rgba(255, 255, 255, 0.5);
  font-size: 1.5rem;
  cursor: pointer;
  transition: all 0.3s ease;
  z-index: 10;
}

.close-btn:hover {
  color: white;
  transform: rotate(90deg);
}

.auth-view {
  display: flex;
  flex-direction: column;
}

h2 {
  margin: 0 0 5px 0;
  font-size: 1.8rem;
  font-weight: 700;
  background: linear-gradient(135deg, #00f2fe 0%, #4facfe 100%);
  -webkit-background-clip: text;
  background-clip: text;
  -webkit-text-fill-color: transparent;
}

.subtitle {
  margin: 0 0 30px 0;
  font-size: 0.9rem;
  color: rgba(255, 255, 255, 0.6);
}

.input-group {
  position: relative;
  margin-bottom: 20px;
}

.input-icon {
  position: absolute;
  left: 15px;
  top: 50%;
  transform: translateY(-50%);
  font-size: 1.2rem;
  color: rgba(255, 255, 255, 0.4);
}

input {
  width: 100%;
  background: rgba(0, 0, 0, 0.2);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 12px;
  padding: 12px 15px 12px 45px;
  color: white;
  font-size: 1rem;
  outline: none;
  transition: all 0.3s ease;
  box-sizing: border-box;
}

input:focus {
  border-color: #4facfe;
  background: rgba(0, 0, 0, 0.4);
  box-shadow: 0 0 0 3px rgba(79, 172, 254, 0.2);
}

.actions-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 25px;
  font-size: 0.9rem;
}

.text-link {
  color: rgba(255, 255, 255, 0.6);
  text-decoration: none;
  transition: color 0.3s ease;
}

.text-link:hover, .text-link.accent {
  color: #4facfe;
}

.submit-btn {
  width: 100%;
  background: linear-gradient(135deg, #00f2fe 0%, #4facfe 100%);
  color: white;
  border: none;
  border-radius: 12px;
  padding: 14px;
  font-size: 1.1rem;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  margin-bottom: 20px;
}

.submit-btn:hover {
  transform: translateY(-2px);
  box-shadow: 0 5px 15px rgba(79, 172, 254, 0.4);
}

.submit-btn:active {
  transform: translateY(0);
}

.switch-hint {
  text-align: center;
  font-size: 0.9rem;
  color: rgba(255, 255, 255, 0.6);
  margin: 0;
}

/* Checkbox Styling */
.checkbox-container {
  display: flex;
  align-items: center;
  gap: 8px;
  cursor: pointer;
  color: rgba(255, 255, 255, 0.6);
  user-select: none;
}

.checkbox-container input {
  position: absolute;
  opacity: 0;
  cursor: pointer;
  height: 0;
  width: 0;
}

.checkmark {
  width: 18px;
  height: 18px;
  background: rgba(0, 0, 0, 0.2);
  border: 1px solid rgba(255, 255, 255, 0.2);
  border-radius: 4px;
  display: flex;
  justify-content: center;
  align-items: center;
  transition: all 0.3s ease;
}

.checkbox-container:hover input ~ .checkmark {
  border-color: #4facfe;
}

.checkbox-container input:checked ~ .checkmark {
  background: #4facfe;
  border-color: #4facfe;
}

.checkmark:after {
  content: "";
  display: none;
  width: 4px;
  height: 8px;
  border: solid white;
  border-width: 0 2px 2px 0;
  transform: rotate(45deg);
  margin-bottom: 2px;
}

.checkbox-container input:checked ~ .checkmark:after {
  display: block;
}

/* Animations */
.fade-slide-enter-active,
.fade-slide-leave-active {
  transition: opacity 0.3s ease, transform 0.3s ease;
}

.fade-slide-enter-from {
  opacity: 0;
  transform: translateX(20px);
}

.fade-slide-leave-to {
  opacity: 0;
  transform: translateX(-20px);
}
</style>
