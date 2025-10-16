from fastapi import APIRouter

from dbs_assignment.endpoints import status, v2methods, v3methods

router = APIRouter()
router.include_router(v2methods.router, tags=["v2methods"])
router.include_router(status.router, tags=["status"])
router.include_router(v3methods.router, tags=["v3methods"])
