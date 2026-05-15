<template>
  <!-- Toast 弹窗通知 -->
  <Transition name="toast">
    <div v-if="feedbackMsg" :class="['auth-toast', feedbackType]">
      <div class="auth-toast-content">
        <Icon 
          :icon="feedbackType === 'success' ? 'ph:check-circle-bold' : 'ph:warning-bold'" 
          width="24" height="24" class="auth-toast-icon" 
        />
        <p>{{ feedbackMsg }}</p>
      </div>
    </div>
  </Transition>

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
            <Icon icon="ph:envelope-simple-bold" class="input-icon" />
            <input type="email" v-model="loginForm.email" placeholder="邮箱地址" />
          </div>
          
          <div class="input-group">
            <Icon icon="ph:lock-key-bold" class="input-icon" />
            <input :type="showPassword ? 'text' : 'password'" v-model="loginForm.password" placeholder="密码" />
            <Icon 
              :icon="showPassword ? 'ph:eye-bold' : 'ph:eye-slash-bold'" 
              class="password-toggle" 
              @click="showPassword = !showPassword" 
            />
          </div>
          
          <div class="actions-row">
            <label class="checkbox-container">
              <input type="checkbox" v-model="loginForm.remember" />
              <span class="checkmark"></span>
              记住我
            </label>
            <a href="#" class="text-link" @click.prevent="switchView('forgot')">忘记密码？</a>
          </div>
          
          <button class="submit-btn" @click="handleLogin" :disabled="isLoading">
            <Icon v-if="isLoading" icon="svg-spinners:ring-resize" />
            <span v-else>登 录</span>
          </button>
          
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
            <input type="text" v-model="registerForm.nickname" placeholder="昵称" />
          </div>
          
          <div class="input-group">
            <Icon icon="ph:envelope-simple-bold" class="input-icon" />
            <input type="email" v-model="registerForm.email" placeholder="邮箱地址" />
          </div>

          <div class="input-group code-group">
            <Icon icon="ph:shield-check-bold" class="input-icon" />
            <input type="text" v-model="registerForm.code" placeholder="验证码" />
            <button class="send-code-btn" @click="sendCode('register')" :disabled="registerCooldown > 0">
              {{ registerCooldown > 0 ? `${registerCooldown}s` : '发送验证码' }}
            </button>
          </div>
          
          <div class="input-group">
            <Icon icon="ph:lock-key-bold" class="input-icon" />
            <input :type="showPassword ? 'text' : 'password'" v-model="registerForm.password" placeholder="密码" />
            <Icon 
              :icon="showPassword ? 'ph:eye-bold' : 'ph:eye-slash-bold'" 
              class="password-toggle" 
              @click="showPassword = !showPassword" 
            />
          </div>

          <div class="input-group">
            <Icon icon="ph:lock-key-bold" class="input-icon" />
            <input :type="showConfirmPassword ? 'text' : 'password'" v-model="registerForm.confirmPassword" placeholder="确认密码" />
            <Icon 
              :icon="showConfirmPassword ? 'ph:eye-bold' : 'ph:eye-slash-bold'" 
              class="password-toggle" 
              @click="showConfirmPassword = !showConfirmPassword" 
            />
          </div>
          
          <button class="submit-btn" @click="handleRegister" :disabled="isLoading">
            <Icon v-if="isLoading" icon="svg-spinners:ring-resize" />
            <span v-else>注 册</span>
          </button>
          
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
            <input type="email" v-model="forgotForm.email" placeholder="注册邮箱地址" />
          </div>

          <div class="input-group code-group">
            <Icon icon="ph:shield-check-bold" class="input-icon" />
            <input type="text" v-model="forgotForm.code" placeholder="验证码" />
            <button class="send-code-btn" @click="sendCode('forgot')" :disabled="forgotCooldown > 0">
              {{ forgotCooldown > 0 ? `${forgotCooldown}s` : '发送验证码' }}
            </button>
          </div>

          <div class="input-group">
            <Icon icon="ph:lock-key-bold" class="input-icon" />
            <input :type="showPassword ? 'text' : 'password'" v-model="forgotForm.newPassword" placeholder="新密码" />
            <Icon 
              :icon="showPassword ? 'ph:eye-bold' : 'ph:eye-slash-bold'" 
              class="password-toggle" 
              @click="showPassword = !showPassword" 
            />
          </div>
          
          <button class="submit-btn" @click="handleResetPassword" :disabled="isLoading">
            <Icon v-if="isLoading" icon="svg-spinners:ring-resize" />
            <span v-else>重置密码</span>
          </button>
          
          <p class="switch-hint">
            记起密码了？ <a href="#" class="text-link accent" @click.prevent="switchView('login')">返回登录</a>
          </p>
        </div>
      </transition>
    </div>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted } from 'vue';
import { Icon } from '@iconify/vue';

const API_BASE = 'http://192.168.52.131:8082';

const emit = defineEmits(['close']);
const currentView = ref('login');
const showPassword = ref(false);
const showConfirmPassword = ref(false);
const isLoading = ref(false);
const feedbackMsg = ref('');
const feedbackType = ref('error'); // 'error' | 'success'

const loginForm = reactive({ email: '', password: '', remember: false });
const registerForm = reactive({ nickname: '', email: '', code: '', password: '', confirmPassword: '' });
const forgotForm = reactive({ email: '', code: '', newPassword: '' });

const registerCooldown = ref(0);
const forgotCooldown = ref(0);
let registerTimer = null;
let forgotTimer = null;

const showFeedback = (msg, type = 'error') => {
  feedbackMsg.value = msg;
  feedbackType.value = type;
  setTimeout(() => { feedbackMsg.value = ''; }, 3000);
};

const switchView = (view) => {
  currentView.value = view;
  showPassword.value = false;
  showConfirmPassword.value = false;
  feedbackMsg.value = '';
};

onMounted(async () => {
  const token = localStorage.getItem('auth_token');
  if (!token) return;

  try {
    const res = await fetch(`${API_BASE}/api/verify_token`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ token }),
    });
    const data = await res.json();

    if (res.ok && data.code === 0) {
      const userInfo = data.data;
      if (userInfo) {
        if (userInfo.nickname) localStorage.setItem('nickname', userInfo.nickname);
        if (userInfo.avatar_url) localStorage.setItem('avatar_url', userInfo.avatar_url);
        if (userInfo.user_id) localStorage.setItem('user_id', userInfo.user_id);
        if (userInfo.email) localStorage.setItem('user_email', userInfo.email);
      }
      showFeedback('已自动登录', 'success');
      setTimeout(() => emit('close'), 800);
    } else {
      localStorage.removeItem('auth_token');
      localStorage.removeItem('user_id');
      localStorage.removeItem('nickname');
      localStorage.removeItem('avatar_url');
      localStorage.removeItem('user_email');
    }
  } catch (err) {
    console.error('Token 校验失败:', err);
  }
});

const sendCode = async (scene) => {
  const email = scene === 'register' ? registerForm.email : forgotForm.email;
  if (!email) {
    showFeedback('请先输入邮箱地址');
    return;
  }

  try {
    const res = await fetch(`${API_BASE}/api/send_code`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        email,
        type: scene === 'register' ? 0 : 1,
      }),
    });
    const data = await res.json();

    if (res.ok && data.code === 0) {
      showFeedback('验证码已发送，请查看邮箱', 'success');
      if (scene === 'register') {
        registerCooldown.value = 60;
        registerTimer = setInterval(() => {
          registerCooldown.value--;
          if (registerCooldown.value <= 0) clearInterval(registerTimer);
        }, 1000);
      } else {
        forgotCooldown.value = 60;
        forgotTimer = setInterval(() => {
          forgotCooldown.value--;
          if (forgotCooldown.value <= 0) clearInterval(forgotTimer);
        }, 1000);
      }
    } else {
      showFeedback(data.msg || '发送失败，请稍后重试');
    }
  } catch (err) {
    showFeedback('网络错误，请检查连接');
  }
};

const handleLogin = async () => {
  if (!loginForm.email || !loginForm.password) {
    showFeedback('请填写邮箱和密码');
    return;
  }
  isLoading.value = true;
  try {
    const res = await fetch(`${API_BASE}/api/login`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        email: loginForm.email,
        password: loginForm.password,
      }),
    });
    const data = await res.json();

    if (res.ok && data.code === 0) {
      showFeedback('登录成功！', 'success');
      const userInfo = data.data;
      if (userInfo) {
        if (userInfo.token) localStorage.setItem('auth_token', userInfo.token);
        if (userInfo.user_id) localStorage.setItem('user_id', userInfo.user_id);
        if (userInfo.nickname) localStorage.setItem('nickname', userInfo.nickname);
        if (userInfo.avatar_url) localStorage.setItem('avatar_url', userInfo.avatar_url);
        if (userInfo.email) localStorage.setItem('user_email', userInfo.email);
      }
      setTimeout(() => emit('close'), 800);
    } else {
      showFeedback(data.msg || '登录失败，请检查账号密码');
    }
  } catch (err) {
    showFeedback('网络错误，请检查连接');
  } finally {
    isLoading.value = false;
  }
};

const handleRegister = async () => {
  if (!registerForm.nickname || !registerForm.email || !registerForm.password || !registerForm.code) {
    showFeedback('请填写所有必填项');
    return;
  }
  if (registerForm.password !== registerForm.confirmPassword) {
    showFeedback('两次密码输入不一致');
    return;
  }
  isLoading.value = true;
  try {
    const res = await fetch(`${API_BASE}/api/register`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        nickname: registerForm.nickname,
        email: registerForm.email,
        password: registerForm.password,
        code: registerForm.code,
      }),
    });
    const data = await res.json();

    if (res.ok && data.code === 0) {
      showFeedback('注册成功！请登录', 'success');
      setTimeout(() => switchView('login'), 1200);
    } else {
      showFeedback(data.msg || '注册失败，请稍后重试');
    }
  } catch (err) {
    showFeedback('网络错误，请检查连接');
  } finally {
    isLoading.value = false;
  }
};

const handleResetPassword = async () => {
  if (!forgotForm.email || !forgotForm.code || !forgotForm.newPassword) {
    showFeedback('请填写所有必填项');
    return;
  }
  isLoading.value = true;
  try {
    const res = await fetch(`${API_BASE}/api/reset_password`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        email: forgotForm.email,
        code: forgotForm.code,
        new_password: forgotForm.newPassword,
      }),
    });
    const data = await res.json();

    if (res.ok && data.code === 0) {
      showFeedback('密码重置成功！请登录', 'success');
      setTimeout(() => switchView('login'), 1200);
    } else {
      showFeedback(data.msg || '重置失败，请检查验证码');
    }
  } catch (err) {
    showFeedback('网络错误，请检查连接');
  } finally {
    isLoading.value = false;
  }
};
</script>

<style scoped>
.auth-modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
  /* 移除黑色背景和模糊遮罩，保持周围透明 */
  background: transparent;
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 2000; /* 高于 Navbar */
}

.auth-card-wrapper {
  position: relative;
  width: 400px;
  /* 极低的颜色不透明度 */
  background: rgba(255, 255, 255, 0.05);
  /* 通过极强的模糊效果（如 40px）来实现视觉上的"看不透（不透明）" */
  backdrop-filter: blur(40px);
  -webkit-backdrop-filter: blur(40px);
  /* 强化边缘的高光，让玻璃感更真实 */
  border: 1px solid rgba(255, 255, 255, 0.15);
  border-top: 1px solid rgba(255, 255, 255, 0.25); /* 顶部高光更亮 */
  border-radius: 24px;
  padding: 50px 40px;
  /* 加入内阴影（模拟玻璃厚度）和外阴影 */
  box-shadow: 0 10px 40px 0 rgba(0, 0, 0, 0.5), inset 0 1px 0 0 rgba(255, 255, 255, 0.1);
  color: white;
  overflow: hidden;
  transition: height 0.4s cubic-bezier(0.25, 0.8, 0.25, 1);
  animation: fadeIn 0.8s ease-out;
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

/* --- Toast 弹窗通知（与 ToastCard.vue 风格一致） --- */
.auth-toast {
  position: fixed;
  top: 20%;
  left: 50%;
  transform: translateX(-50%);
  z-index: 10000;
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
  padding: 16px;
  border-radius: 12px;
  /* min-width: 280px; */
}

.auth-toast.error {
  background: rgba(255, 69, 58, 0.1);
  border: 1px solid rgba(255, 69, 58, 0.4);
}

.auth-toast.success {
  background: rgba(0, 242, 254, 0.1);
  border: 1px solid rgba(0, 242, 254, 0.4);
}

.auth-toast-content {
  display: flex;
  align-items: center;
  gap: 12px;
  color: white;
  font-weight: 400;
  font-size: 1.3rem;
  letter-spacing: 1px;
}

.auth-toast.error .auth-toast-icon {
  color: rgba(255, 69, 58, 0.9);
}

.auth-toast.success .auth-toast-icon {
  color: #00f2fe;
}

.auth-toast-content p {
  margin: 0;
}

/* Toast 动画 */
.toast-enter-active {
  transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
}

.toast-leave-active {
  transition: all 0.3s cubic-bezier(0.4, 0, 1, 1);
}

.toast-enter-from {
  opacity: 0;
  transform: translate(-50%, 20px);
}

.toast-leave-to {
  opacity: 0;
  transform: translate(-50%, -40px);
}

.auth-view {
  display: flex;
  flex-direction: column;
}

h2 {
  margin: 0 0 12px 0;
  font-size: 1.8rem;
  font-weight: 700;
  color: white;
  letter-spacing: 1px;
  text-align: center;
}

.subtitle {
  margin: 0 0 32px 0;
  font-size: 1.1rem;
  color: rgba(255, 255, 255, 0.6);
  text-align: center;
  line-height: 1.6;
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
  pointer-events: none;
}

.password-toggle {
  position: absolute;
  right: 15px;
  top: 50%;
  transform: translateY(-50%);
  font-size: 1.2rem;
  color: rgba(255, 255, 255, 0.4);
  cursor: pointer;
  transition: all 0.3s ease;
  z-index: 2;
}

.password-toggle:hover {
  color: white;
}

input {
  width: 100%;
  background: rgba(0, 0, 0, 0.2);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 12px;
  padding: 12px 45px 12px 45px;
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

/* 解决浏览器自动填充导致输入框变白的问题 */
input:-webkit-autofill,
input:-webkit-autofill:hover, 
input:-webkit-autofill:focus, 
input:-webkit-autofill:active {
  /* 通过超长过渡时间"冻结"背景色，保留原本的透明毛玻璃背景 */
  transition: background-color 5000s ease-in-out 0s;
  /* 确保自动填充后的文字颜色依然是白色 */
  -webkit-text-fill-color: white !important;
  /* 修复自动填充时光标变黑的问题 */
  caret-color: white !important;
  /* 保持与原输入框一致的字体 */
  font-family: inherit !important;
}

/* --- 验证码输入组 --- */
.code-group input {
  padding-right: 120px;
}

.send-code-btn {
  position: absolute;
  right: 6px;
  top: 50%;
  transform: translateY(-50%);
  padding: 6px 14px;
  background: linear-gradient(to right, #4facfe, #00f2fe);
  color: white;
  border: none;
  border-radius: 8px;
  font-size: 0.8rem;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  white-space: nowrap;
}

.send-code-btn:hover:not(:disabled) {
  box-shadow: 0 2px 10px rgba(79, 172, 254, 0.4);
}

.send-code-btn:disabled {
  background: rgba(255, 255, 255, 0.1);
  color: rgba(255, 255, 255, 0.4);
  cursor: not-allowed;
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
  background: linear-gradient(to right, #4facfe, #00f2fe);
  color: white;
  border: none;
  border-radius: 12px;
  padding: 14px;
  font-size: 1.1rem;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  margin-bottom: 20px;
  box-shadow: 0 4px 15px rgba(79, 172, 254, 0.3);
  display: flex;
  justify-content: center;
  align-items: center;
}

.submit-btn:hover:not(:disabled) {
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(79, 172, 254, 0.4);
}

.submit-btn:active:not(:disabled) {
  transform: translateY(0);
}

.submit-btn:disabled {
  opacity: 0.7;
  cursor: not-allowed;
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

@keyframes fadeIn {
  from { opacity: 0; transform: translateY(20px); }
  to { opacity: 1; transform: translateY(0); }
}
</style>
