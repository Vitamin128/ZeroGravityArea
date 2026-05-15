<template>
  <div class="auth-view">
    <h2>欢迎回来</h2>
    <p class="subtitle">登录您的 Zero Gravity Area 账号</p>
    
    <div class="input-group">
      <Icon icon="ph:envelope-simple-bold" class="input-icon" />
      <input type="email" v-model="form.email" placeholder="邮箱地址" @keyup.enter="handleLogin" />
    </div>
    
    <div class="input-group">
      <Icon icon="ph:lock-key-bold" class="input-icon" />
      <input :type="showPassword ? 'text' : 'password'" v-model="form.password" placeholder="密码" @keyup.enter="handleLogin" />
      <Icon 
        :icon="showPassword ? 'ph:eye-bold' : 'ph:eye-slash-bold'" 
        class="password-toggle" 
        @click="showPassword = !showPassword" 
      />
    </div>
    
    <div class="actions-row">
      <div class="spacer"></div>
      <a href="#" class="text-link" @click.prevent="$emit('switch', 'forgot')">忘记密码？</a>
    </div>
    
    <button class="submit-btn" @click="handleLogin" :disabled="isLoading">
      <Icon v-if="isLoading" icon="svg-spinners:ring-resize" />
      <span v-else>登 录</span>
    </button>
    
    <p class="switch-hint">
      还没有账号？ <a href="#" class="text-link accent" @click.prevent="$emit('switch', 'register')">立即注册</a>
    </p>
  </div>
</template>

<script setup>
import { ref, reactive } from 'vue';
import { Icon } from '@iconify/vue';
import { login } from '@/api/auth';

const emit = defineEmits(['switch', 'success', 'error']);
const isLoading = ref(false);
const showPassword = ref(false);

const form = reactive({
  email: '',
  password: ''
});

const handleLogin = async () => {
  if (!form.email || !form.password) {
    emit('error', '请填写邮箱和密码');
    return;
  }
  isLoading.value = true;
  try {
    const data = await login({
      email: form.email,
      password: form.password,
    });
    
    // 登录成功处理
    const userInfo = data.data;
    if (userInfo) {
      if (userInfo.token) localStorage.setItem('auth_token', userInfo.token);
      if (userInfo.user_id) localStorage.setItem('user_id', userInfo.user_id);
      if (userInfo.nickname) localStorage.setItem('nickname', userInfo.nickname);
      if (userInfo.avatar_url) localStorage.setItem('avatar_url', userInfo.avatar_url);
      if (userInfo.email) localStorage.setItem('user_email', userInfo.email);
    }
    
    emit('success', '登录成功！');
  } catch (err) {
    emit('error', err.message || '登录失败，请检查账号密码');
  } finally {
    isLoading.value = false;
  }
};
</script>

<style scoped>
.auth-view { display: flex; flex-direction: column; }
.auth-view h2 { margin: 0 0 12px 0; font-size: 1.8rem; text-align: center; color: white; }
.auth-view .subtitle { margin: 0 0 32px 0; font-size: 1.1rem; color: rgba(255, 255, 255, 0.6); text-align: center; }

.input-group { position: relative; margin-bottom: 20px; }
.input-icon { position: absolute; left: 15px; top: 50%; transform: translateY(-50%); color: rgba(255, 255, 255, 0.4); pointer-events: none; }
.password-toggle { position: absolute; right: 15px; top: 50%; transform: translateY(-50%); color: rgba(255, 255, 255, 0.4); cursor: pointer; z-index: 2; }

input {
  width: 100%;
  background: rgba(0, 0, 0, 0.2);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 12px;
  padding: 12px 45px;
  color: white;
  outline: none;
  transition: all 0.3s ease;
  box-sizing: border-box;
}
input:focus { border-color: #4facfe; background: rgba(0, 0, 0, 0.4); }

/* 解决浏览器自动填充导致输入框变白的问题 */
input:-webkit-autofill,
input:-webkit-autofill:hover, 
input:-webkit-autofill:focus, 
input:-webkit-autofill:active {
  transition: background-color 5000s ease-in-out 0s;
  -webkit-text-fill-color: white !important;
  caret-color: white !important;
}

.submit-btn {
  width: 100%;
  background: linear-gradient(to right, #4facfe, #00f2fe);
  color: white; border: none; border-radius: 12px; padding: 14px;
  font-size: 1.1rem; font-weight: 600; cursor: pointer;
  margin-bottom: 20px;
  display: flex; justify-content: center; align-items: center;
  transition: all 0.3s ease;
}
.submit-btn:hover:not(:disabled) { transform: translateY(-2px); box-shadow: 0 5px 15px rgba(79, 172, 254, 0.4); }
.submit-btn:disabled { opacity: 0.7; cursor: not-allowed; }

.actions-row { display: flex; justify-content: space-between; align-items: center; margin-bottom: 25px; font-size: 0.9rem; }
.spacer { flex: 1; }
.text-link { color: rgba(255, 255, 255, 0.6); text-decoration: none; transition: color 0.3s ease; }
.text-link:hover, .text-link.accent { color: #4facfe; }

.switch-hint { text-align: center; font-size: 0.9rem; color: rgba(255, 255, 255, 0.6); }
</style>
