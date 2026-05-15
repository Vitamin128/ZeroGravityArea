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
      <label class="checkbox-container">
        <input type="checkbox" v-model="form.remember" />
        <span class="checkmark"></span>
        记住我
      </label>
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
  password: '',
  remember: false
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
