from django.urls import path
from . import auth_views

urlpatterns = [
    path('register/', auth_views.register_user, name='register'),
    path('login/', auth_views.login_user, name='login'),
    path('logout/', auth_views.logout_user, name='logout'),
    path('me/', auth_views.get_user_info, name='user-info'),
    path('profile/', auth_views.update_profile, name='update-profile'),  # 
    path('change-password/', auth_views.change_password, name='change-password'),  # 
    # Queda expuesto como /api/auth/notification-settings/
    path('notification-settings/', auth_views.update_notification_settings, name='notification-settings'),
    # Flujo de recuperación de contraseña
    path('password-reset/', auth_views.password_reset_request, name='password-reset'),
    path('password-reset-confirm/', auth_views.password_reset_confirm, name='password-reset-confirm'),
]