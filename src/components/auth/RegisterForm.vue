<template>
  <div class="auth-view">
    <h2>创建账号</h2>
    <p class="subtitle">加入 Zero Gravity Area</p>
    
    <div class="input-group">
      <Icon icon="ph:user-bold" class="input-icon" />
      <input type="text" v-model="form.nickname" placeholder="昵称" />
    </div>
    
    <div class="input-group">
      <Icon icon="ph:envelope-simple-bold" class="input-icon" />
      <input type="email" v-model="form.email" placeholder="邮箱地址" />
    </div>

    <div class="input-group code-group">
      <Icon icon="ph:shield-check-bold" class="input-icon" />
      <input type="text" v-model="form.code" placeholder="验证码" />
      <button class="send-code-btn" @click="handleSendCode" :disabled="cooldown > 0">
        {{ cooldown > 0 ? `${cooldown}s` : '发送验证码' }}
      </button>
    </div>
    
    <div class="input-group">
      <Icon icon="ph:lock-key-bold" class="input-icon" />
      <input :type="showPassword ? 'text' : 'password'" v-model="form.password" placeholder="密码" />
      <Icon 
        :icon="showPassword ? 'ph:eye-bold' : 'ph:eye-slash-bold'" 
        class="password-toggle" 
        @click="showPassword = !showPassword" 
      />
    </div>

    <div class="input-group">
      <Icon icon="ph:lock-key-bold" class="input-icon" />
      <input :type="showConfirmPassword ? 'text' : 'password'" v-model="form.confirmPassword" placeholder="确认密码" />
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
      已有账号？ <a href="#" class="text-link accent" @click.prevent="$emit('switch', 'login')">返回登录</a>
    </p>
  </div>
</template>

<script setup>
import { ref, reactive, onUnmounted } from 'vue';
import { Icon } from '@iconify/vue';
import { sendCode } from '@/api/auth';
import request from '@/utils/request';

const emit = defineEmits(['switch', 'success', 'error']);
const isLoading = ref(false);
const showPassword = ref(false);
const showConfirmPassword = ref(false);
const cooldown = ref(0);
let timer = null;

const form = reactive({
  nickname: '',
  email: '',
  code: '',
  password: '',
  confirmPassword: ''
});

const handleSendCode = async () => {
  if (!form.email) {
    emit('error', '请先输入邮箱地址');
    return;
  }
  try {
    await sendCode(form.email, 0); // 0 为注册场景
    emit('success', '验证码已发送，请查看邮箱', 'success');
    cooldown.value = 60;
    timer = setInterval(() => {
      cooldown.value--;
      if (cooldown.value <= 0) clearInterval(timer);
    }, 1000);
  } catch (err) {
    emit('error', err.message || '发送失败，请稍后重试');
  }
};

const handleRegister = async () => {
  if (!form.nickname || !form.email || !form.password || !form.code) {
    emit('error', '请填写所有必填项');
    return;
  }
  if (form.password !== form.confirmPassword) {
    emit('error', '两次密码输入不一致');
    return;
  }
  isLoading.value = true;
  try {
    // 这里可以直接用 request 或者在 api/auth.js 里增加 register 函数
    await request.post('/api/register', {
      nickname: form.nickname,
      email: form.email,
      password: form.password,
      code: form.code,
    });
    emit('success', '注册成功！请登录');
    setTimeout(() => emit('switch', 'login'), 1200);
  } catch (err) {
    emit('error', err.message || '注册失败，请稍后重试');
  } finally {
    isLoading.value = false;
  }
};

onUnmounted(() => {
  if (timer) clearInterval(timer);
});
</script>
