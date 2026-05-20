import request from '@/utils/request';

// 登录接口
export function login(data) {
    return request.post('/api/login', data);
}

// 注册接口
export function register(data) {
    return request.post('/api/register', data);
}

// 发送验证码 (0: 注册, 1: 找回密码)
export function sendCode(email, type) {
    return request.post('/api/send_code', { email, type });
}

// 重置密码接口
export function resetPassword(data) {
    return request.post('/api/reset_password', data);
}

// 验证 Token 是否有效
export function verifyToken(token) {
    return request.post('/api/verify_token', { token });
}