from django.urls import path
from rest_framework.routers import DefaultRouter
from .views import TratamientoViewSet, HistorialTomaViewSet, registrar_toma

router = DefaultRouter()
router.register(r"tratamientos", TratamientoViewSet)
router.register(r"historial", HistorialTomaViewSet)

urlpatterns = router.urls + [
    path('registrar-toma/', registrar_toma, name='registrar-toma'),
]