<template>
  <div class="auth-view">
    <h2>欢迎回来</h2>
    <p class="subtitle">登录您的账号</p>

    <div class="input-group">
      <Icon icon="ph:envelope-simple" class="input-icon" />
      <input type="email" v-model="form.email" placeholder="邮箱地址" @keyup.enter="handleLogin" />
    </div>

    <div class="input-group">
      <Icon icon="ph:lock-key" class="input-icon" />
      <input :type="showPassword ? 'text' : 'password'" v-model="form.password" placeholder="密码" @keyup.enter="handleLogin" />
      <Icon
        :icon="showPassword ? 'ph:eye' : 'ph:eye-slash'"
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
      <span v-else>登录</span>
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
.auth-view {
  display: flex;
  flex-direction: column;
  font-family: 'Inter', sans-serif;
}

.auth-view h2 {
  margin: 0 0 8px 0;
  font-size: 1.6rem;
  text-align: center;
  color: #2C2C2C;
  font-weight: 600;
}

.auth-view .subtitle {
  margin: 0 0 32px 0;
  font-size: 0.95rem;
  color: #6B6B6B;
  text-align: center;
}

.input-group {
  position: relative;
  margin-bottom: 16px;
}

.input-icon {
  position: absolute;
  left: 14px;
  top: 50%;
  transform: translateY(-50%);
  color: #6B6B6B;
  pointer-events: none;
  font-size: 1.1rem;
}

.password-toggle {
  position: absolute;
  right: 14px;
  top: 50%;
  transform: translateY(-50%);
  color: #6B6B6B;
  cursor: pointer;
  z-index: 2;
  font-size: 1.1rem;
  transition: color 0.2s ease;
}

.password-toggle:hover {
  color: #2C2C2C;
}

input {
  width: 100%;
  background: #FFFFFF;
  border: 1px solid #E8E3DA;
  border-radius: 4px;
  padding: 12px 42px;
  color: #2C2C2C;
  outline: none;
  transition: all 0.2s ease;
  box-sizing: border-box;
  font-family: 'Lora', serif;
  font-size: 0.95rem;
}

input:focus {
  border-color: #8B6F47;
}

input::placeholder {
  color: #B8B8B8;
}

.actions-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 24px;
  font-size: 0.85rem;
}

.spacer {
  flex: 1;
}

.text-link {
  color: #6B6B6B;
  text-decoration: none;
  transition: color 0.2s ease;
  position: relative;
}

.text-link::after {
  content: '';
  position: absolute;
  bottom: -2px;
  left: 0;
  width: 0;
  height: 1px;
  background: #8B6F47;
  transition: width 0.2s ease;
}

.text-link:hover::after,
.text-link.accent::after {
  width: 100%;
}

.text-link:hover,
.text-link.accent {
  color: #8B6F47;
}

.submit-btn {
  width: 100%;
  background: #8B6F47;
  color: white;
  border: none;
  border-radius: 4px;
  padding: 12px;
  font-size: 0.95rem;
  font-weight: 500;
  cursor: pointer;
  margin-bottom: 20px;
  display: flex;
  justify-content: center;
  align-items: center;
  transition: all 0.2s ease;
  font-family: 'Inter', sans-serif;
}

.submit-btn:hover:not(:disabled) {
  background: #6B5437;
}

.submit-btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.switch-hint {
  text-align: center;
  font-size: 0.85rem;
  color: #6B6B6B;
  margin: 0;
}
</style>
