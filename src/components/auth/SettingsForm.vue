<template>
  <div class="auth-view">
    <h2>个人信息</h2>
    <p class="subtitle">管理您的账号设置</p>

    <!-- 上半部分：左头像 + 右表单 -->
    <div class="settings-top">
      <!-- 左侧：头像 -->
      <div class="avatar-section">
        <div class="avatar-preview-wrapper">
          <img :src="avatarUrl || defaultAvatar" @error="(e) => e.target.src = defaultAvatar" alt="Avatar" class="large-avatar" />
          <div class="avatar-edit-overlay" @click="triggerAvatarUpload">
            <Icon icon="ph:camera" />
          </div>
        </div>
      </div>

      <!-- 右侧：邮箱 + 用户名 -->
      <div class="form-section">
        <div class="input-group">
          <Icon icon="ph:envelope-simple" class="input-icon" />
          <input type="text" :value="userEmail" readonly disabled />
        </div>
        <span class="input-hint">邮箱不可更改</span>

        <div class="input-group">
          <Icon icon="ph:user" class="input-icon" />
          <input type="text" v-model="nickname" placeholder="输入您的昵称" maxlength="20" />
        </div>
        <span class="input-hint">最多 20 个字符</span>
      </div>
    </div>

    <!-- 下半部分：三个按钮 -->
    <div class="settings-actions">
      <button class="action-btn outline-btn" @click="triggerAvatarUpload">
        <Icon icon="ph:camera" />
        <span>更换头像</span>
      </button>
      <button class="action-btn save-btn" @click="handleSave" :disabled="isLoading">
        <Icon v-if="isLoading" icon="svg-spinners:ring-resize" />
        <template v-else>
          <Icon icon="ph:floppy-disk" />
          <span>保存修改</span>
        </template>
      </button>
      <button class="action-btn logout-btn" @click="handleLogout">
        <Icon icon="ph:sign-out" />
        <span>退出登录</span>
      </button>
    </div>

    <!-- 隐藏的文件上传 input -->
    <input type="file" ref="fileInput" @change="onFileChange" accept="image/*" style="display: none" />
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue';
import { Icon } from '@iconify/vue';
import defaultAvatar from '@/assets/white_user.png';
import request from '@/utils/request'; // 确保导入了封装好的 axios 实例

const emit = defineEmits(['close', 'success', 'error']);
const isLoading = ref(false);

// 响应式数据
const nickname = ref(localStorage.getItem('nickname') || '');
const userEmail = ref(localStorage.getItem('user_email') || '未绑定邮箱');
const avatarUrl = ref(localStorage.getItem('avatar_url') || '');

// 文件上传相关
const fileInput = ref(null);
const selectedFile = ref(null);

const triggerAvatarUpload = () => fileInput.value.click();

// 当用户选择文件时：生成预览图并保存文件对象
const onFileChange = (e) => {
  const file = e.target.files[0];
  if (!file) return;

  // 1. 保存文件对象，稍后提交
  selectedFile.value = file;

  // 2. 生成本地预览图
  const reader = new FileReader();
  reader.onload = (event) => {
    avatarUrl.value = event.target.result;
  };
  reader.readAsDataURL(file);
};

// 提交修改到后端
const handleSave = async () => {
  if (!nickname.value.trim()) {
    emit('error', '昵称不能为空');
    return;
  }

  isLoading.value = true;
  try {
    // 构造 FormData，匹配后端 httplib 的 req.get_file_value("xxx")
    const formData = new FormData();
    formData.append('user_id', localStorage.getItem('user_id') || '');
    formData.append('nickname', nickname.value);
    
    // 如果用户选了新头像，则添加 avatar 字段
    if (selectedFile.value) {
      formData.append('avatar', selectedFile.value);
    }

    // 发送 POST 请求到 http://ip:8082/api/update_profile
    const res = await request.post('/api/update_profile', formData);

    // 根据后端逻辑，code 为 0 表示成功
    if (res.code === 0) {
      // 更新本地持久化数据
      localStorage.setItem('nickname', nickname.value);
      if (res.data && res.data.avatar_url) {
        localStorage.setItem('avatar_url', res.data.avatar_url);
      }
      emit('success', '资料更新成功！');
      
      // 重置文件选择状态
      selectedFile.value = null;
    } else {
      emit('error', res.msg || '更新失败');
    }
  } catch (err) {
    console.error('Update Profile Error:', err);
    emit('error', '服务器请求失败，请检查网络或后端状态');
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
/* === 基础布局 === */
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
  border-radius: 4px;
  overflow: hidden;
  border: 1px solid #E8E3DA;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
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
  background: rgba(139, 111, 71, 0.85);
  display: flex;
  align-items: center;
  justify-content: center;
  color: white;
  font-size: 1.8rem;
  opacity: 0;
  transition: opacity 0.2s ease;
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

/* === 输入框样式 === */
.input-group {
  position: relative;
  margin-bottom: 8px;
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

input:disabled {
  background: #F7F4ED;
  color: #6B6B6B;
  cursor: not-allowed;
}

.input-hint {
  font-size: 0.75rem;
  color: #B8B8B8;
  margin-bottom: 16px;
  padding-left: 4px;
}

/* === 底部三个按钮 === */
.settings-actions {
  display: flex;
  gap: 12px;
  border-top: 1px solid #E8E3DA;
  padding-top: 24px;
}

.action-btn {
  width: 135px;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  padding: 12px 0;
  border-radius: 4px;
  font-size: 0.9rem;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.2s ease;
  border: none;
  font-family: 'Inter', sans-serif;
}

/* 更换头像按钮 —— 轮廓风格 */
.outline-btn {
  background: #FFFFFF;
  border: 1px solid #E8E3DA;
  color: #6B6B6B;
}

.outline-btn:hover {
  background: #F7F4ED;
  border-color: #8B6F47;
  color: #2C2C2C;
}

/* 保存修改按钮 —— 主色 */
.save-btn {
  background: #8B6F47;
  color: white;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
}

.save-btn:hover:not(:disabled) {
  background: #6B5437;
}

.save-btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

/* 退出登录按钮 —— 红色警告风格 */
.logout-btn {
  background: #FFFFFF;
  border: 1px solid #E8E3DA;
  color: #D32F2F;
}

.logout-btn:hover {
  background: #FFF5F5;
  border-color: #D32F2F;
}
</style>
