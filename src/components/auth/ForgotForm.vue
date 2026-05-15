<template>
  <div class="auth-view">
    <h2>找回密码</h2>
    <p class="subtitle">输入您的邮箱以重置密码</p>
    
    <div class="input-group">
      <Icon icon="ph:envelope-simple-bold" class="input-icon" />
      <input type="email" v-model="form.email" placeholder="注册邮箱地址" />
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
      <input :type="showPassword ? 'text' : 'password'" v-model="form.newPassword" placeholder="新密码" />
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
      记起密码了？ <a href="#" class="text-link accent" @click.prevent="$emit('switch', 'login')">返回登录</a>
    </p>
  </div>
</template>

<script setup>
import { ref, reactive, onUnmounted } from 'vue';
import { Icon } from '@iconify/vue';
import { sendCode, resetPassword } from '@/api/auth';

const emit = defineEmits(['switch', 'success', 'error']);
const isLoading = ref(false);
const showPassword = ref(false);
const cooldown = ref(0);
let timer = null;

const form = reactive({
  email: '',
  code: '',
  newPassword: ''
});

const handleSendCode = async () => {
  if (!form.email) {
    emit('error', '请先输入邮箱地址');
    return;
  }
  try {
    await sendCode(form.email, 1); // 1 为找回密码场景
    emit('success', '验证码已发送，请查看邮箱');
    cooldown.value = 60;
    timer = setInterval(() => {
      cooldown.value--;
      if (cooldown.value <= 0) clearInterval(timer);
    }, 1000);
  } catch (err) {
    emit('error', err.message || '发送失败，请稍后重试');
  }
};

const handleResetPassword = async () => {
  if (!form.email || !form.code || !form.newPassword) {
    emit('error', '请填写所有必填项');
    return;
  }
  isLoading.value = true;
  try {
    await resetPassword({
      email: form.email,
      code: form.code,
      new_password: form.newPassword,
    });
    emit('success', '密码重置成功！请登录');
    setTimeout(() => emit('switch', 'login'), 1200);
  } catch (err) {
    emit('error', err.message || '重置失败，请检查验证码');
  } finally {
    isLoading.value = false;
  }
};

onUnmounted(() => {
  if (timer) clearInterval(timer);
});
</script>
