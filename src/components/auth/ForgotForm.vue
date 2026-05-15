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

.code-group input { padding-right: 120px; }
.send-code-btn {
  position: absolute; right: 6px; top: 50%; transform: translateY(-50%);
  padding: 6px 14px; background: linear-gradient(to right, #4facfe, #00f2fe);
  color: white; border: none; border-radius: 8px; font-size: 0.8rem; cursor: pointer;
  transition: all 0.3s ease;
}
.send-code-btn:disabled { background: rgba(255, 255, 255, 0.1); color: rgba(255, 255, 255, 0.4); cursor: not-allowed; }

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

.text-link { color: rgba(255, 255, 255, 0.6); text-decoration: none; transition: color 0.3s ease; }
.text-link:hover, .text-link.accent { color: #4facfe; }

.switch-hint { text-align: center; font-size: 0.9rem; color: rgba(255, 255, 255, 0.6); margin-top: 10px; }
</style>

