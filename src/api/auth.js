import request from '@/utils/request';
// 登录接口
export function login(data) {
    return request({
        url: '/api/login',
        method: 'post',
        data
    });
}
// 发送验证码 (支持通过参数切换场景)
export function sendCode(email, type) {
    return request.post('/api/send_code', { email, type });
}
// 验证 Token
export function verifyToken(token) {
    return request.post('/api/verify_token', { token });
}