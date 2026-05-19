<template>
  <div class="auth-view">
    <h2>找回密码</h2>
    <p class="subtitle">输入您的邮箱以重置密码</p>
    
    <div class="input-group">
      <Icon icon="ph:envelope-simple" class="input-icon" />
      <input type="email" v-model="form.email" placeholder="注册邮箱地址" />
    </div>

    <div class="input-group code-group">
      <Icon icon="ph:shield-check" class="input-icon" />
      <input type="text" v-model="form.code" placeholder="验证码" />
      <button class="send-code-btn" @click="handleSendCode" :disabled="cooldown > 0">
        {{ cooldown > 0 ? `${cooldown}s` : '发送验证码' }}
      </button>
    </div>

    <div class="input-group">
      <Icon icon="ph:lock-key" class="input-icon" />
      <input :type="showPassword ? 'text' : 'password'" v-model="form.newPassword" placeholder="新密码" />
      <Icon
        :icon="showPassword ? 'ph:eye' : 'ph:eye-slash'"
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

.code-group input {
  padding-right: 130px;
}

.send-code-btn {
  position: absolute;
  right: 6px;
  top: 50%;
  transform: translateY(-50%);
  padding: 8px 16px;
  background: #8B6F47;
  color: white;
  border: none;
  border-radius: 4px;
  font-size: 0.85rem;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.2s ease;
  font-family: 'Inter', sans-serif;
}

.send-code-btn:hover:not(:disabled) {
  background: #6B5437;
}

.send-code-btn:disabled {
  background: #D4C4B0;
  color: #FFFFFF;
  cursor: not-allowed;
  opacity: 0.6;
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

.switch-hint {
  text-align: center;
  font-size: 0.85rem;
  color: #6B6B6B;
  margin: 0;
}
</style>

