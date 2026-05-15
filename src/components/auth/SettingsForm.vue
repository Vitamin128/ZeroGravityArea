<template>
  <div class="auth-view">
    <h2>个人信息</h2>
    <p class="subtitle">管理您的账号设置</p>

    <!-- 上半部分：左头像 + 右表单 -->
    <div class="settings-top">
      <!-- 左侧：头像 -->
      <div class="avatar-section">
        <div class="avatar-preview-wrapper">
          <img :src="avatarUrl || defaultAvatar" alt="Avatar" class="large-avatar" />
          <div class="avatar-edit-overlay" @click="triggerAvatarUpload">
            <Icon icon="ph:camera-bold" />
          </div>
        </div>
      </div>

      <!-- 右侧：邮箱 + 用户名 -->
      <div class="form-section">
        <div class="input-group">
          <Icon icon="ph:envelope-simple-bold" class="input-icon" />
          <input type="text" :value="userEmail" readonly disabled />
        </div>
        <span class="input-hint">邮箱不可更改</span>

        <div class="input-group">
          <Icon icon="ph:user-bold" class="input-icon" />
          <input type="text" v-model="nickname" placeholder="输入您的昵称" maxlength="20" />
        </div>
        <span class="input-hint">最多 20 个字符</span>
      </div>
    </div>

    <!-- 下半部分：三个按钮 -->
    <div class="settings-actions">
      <button class="action-btn outline-btn" @click="triggerAvatarUpload">
        <Icon icon="ph:camera-bold" />
        <span>更换头像</span>
      </button>
      <button class="action-btn save-btn" @click="handleSave" :disabled="isLoading">
        <Icon v-if="isLoading" icon="svg-spinners:ring-resize" />
        <template v-else>
          <Icon icon="ph:floppy-disk-bold" />
          <span>保存修改</span>
        </template>
      </button>
      <button class="action-btn logout-btn" @click="handleLogout">
        <Icon icon="ph:sign-out-bold" />
        <span>退出登录</span>
      </button>
    </div>

    <!-- 隐藏的文件上传 input -->
    <input type="file" ref="fileInput" @change="onFileChange" accept="image/*" style="display: none" />
  </div>
</template>

<script setup>
import { ref } from 'vue';
import { Icon } from '@iconify/vue';
import defaultAvatar from '@/assets/white_user.png';

const emit = defineEmits(['close', 'success', 'error']);
const isLoading = ref(false);

const nickname = ref(localStorage.getItem('nickname') || '');
const userEmail = ref(localStorage.getItem('user_email') || '未绑定邮箱');
const avatarUrl = ref(localStorage.getItem('avatar_url') || '');
const fileInput = ref(null);

const triggerAvatarUpload = () => fileInput.value.click();

const onFileChange = async (e) => {
  const file = e.target.files[0];
  if (!file) return;
  emit('success', '头像上传功能待接入后端');
};

const handleSave = async () => {
  if (!nickname.value.trim()) {
    emit('error', '昵称不能为空');
    return;
  }
  isLoading.value = true;
  try {
    localStorage.setItem('nickname', nickname.value);
    emit('success', '修改保存成功！');
  } catch (err) {
    emit('error', err.message || '保存失败');
  } finally {
    isLoading.value = false;
  }
};

const handleLogout = () => {
  localStorage.removeItem('auth_token');
  localStorage.removeItem('user_id');
  localStorage.removeItem('nickname');
  localStorage.removeItem('avatar_url');
  localStorage.removeItem('user_email');
  emit('success', '已成功退出登录');
  setTimeout(() => window.location.reload(), 800);
};
</script>

<style scoped>
/* === 基础布局（与 LoginForm 等保持一致） === */
.auth-view {
  display: flex;
  flex-direction: column;
}

.auth-view h2 {
  margin: 0 0 12px 0;
  font-size: 1.8rem;
  text-align: center;
  color: white;
}

.auth-view .subtitle {
  margin: 0 0 32px 0;
  font-size: 1.1rem;
  color: rgba(255, 255, 255, 0.6);
  text-align: center;
}

/* === 上半部分：头像 + 表单 === */
.settings-top {
  display: flex;
  gap: 28px;
  margin-bottom: 28px;
  align-items: flex-start;
}

/* 头像区域 */
.avatar-section {
  flex-shrink: 0;
}

.avatar-preview-wrapper {
  position: relative;
  width: 120px;
  height: 120px;
  border-radius: 16px;
  overflow: hidden;
  border: 1px solid rgba(255, 255, 255, 0.15);
  box-shadow: 0 6px 20px rgba(0, 0, 0, 0.3);
}

.large-avatar {
  width: 100%;
  height: 100%;
  object-fit: cover;
}

.avatar-edit-overlay {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: rgba(0, 0, 0, 0.45);
  display: flex;
  align-items: center;
  justify-content: center;
  color: white;
  font-size: 1.8rem;
  opacity: 0;
  transition: opacity 0.3s ease;
  cursor: pointer;
}

.avatar-preview-wrapper:hover .avatar-edit-overlay {
  opacity: 1;
}

/* 表单区域 */
.form-section {
  flex: 1;
  display: flex;
  flex-direction: column;
}

/* === 输入框样式（与 LoginForm 等完全一致） === */
.input-group {
  position: relative;
  margin-bottom: 8px;
}

.input-icon {
  position: absolute;
  left: 15px;
  top: 50%;
  transform: translateY(-50%);
  color: rgba(255, 255, 255, 0.4);
  pointer-events: none;
}

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

input:focus {
  border-color: #4facfe;
  background: rgba(0, 0, 0, 0.4);
}

input:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

/* 解决浏览器自动填充导致输入框变白的问题 */
input:-webkit-autofill,
input:-webkit-autofill:hover,
input:-webkit-autofill:focus,
input:-webkit-autofill:active {
  transition: background-color 5000s ease-in-out 0s;
  -webkit-text-fill-color: white !important;
  caret-color: white !important;
}

.input-hint {
  font-size: 0.75rem;
  color: rgba(255, 255, 255, 0.35);
  margin-bottom: 16px;
  padding-left: 4px;
}

/* === 底部三个按钮 === */
.settings-actions {
  display: flex;
  gap: 12px;
  border-top: 1px solid rgba(255, 255, 255, 0.08);
  padding-top: 24px;
}

.action-btn {
  /* flex: 1; */
  width: 135px;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  padding: 12px 0;
  border-radius: 12px;
  font-size: 0.95rem;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  border: none;
}

/* 更换头像按钮 —— 轮廓风格 */
.outline-btn {
  background: rgba(255, 255, 255, 0.05);
  border: 1px solid rgba(255, 255, 255, 0.2);
  color: white;
}

.outline-btn:hover {
  background: rgba(255, 255, 255, 0.12);
  border-color: rgba(255, 255, 255, 0.4);
  transform: translateY(-2px);
}

/* 保存修改按钮 —— 主色渐变（与 submit-btn 一致） */
.save-btn {
  background: linear-gradient(to right, #4facfe, #00f2fe);
  color: white;
  box-shadow: 0 4px 15px rgba(79, 172, 254, 0.3);
}

.save-btn:hover:not(:disabled) {
  transform: translateY(-2px);
  box-shadow: 0 6px 20px rgba(79, 172, 254, 0.4);
}

.save-btn:disabled {
  opacity: 0.7;
  cursor: not-allowed;
}

/* 退出登录按钮 —— 红色警告风格 */
.logout-btn {
  background: rgba(255, 69, 58, 0.08);
  border: 1px solid rgba(255, 69, 58, 0.3);
  color: rgba(255, 69, 58, 0.85);
}

.logout-btn:hover {
  background: rgba(255, 69, 58, 0.15);
  border-color: #ff453a;
  color: #ff453a;
  transform: translateY(-2px);
  box-shadow: 0 4px 15px rgba(255, 69, 58, 0.2);
}
</style>
