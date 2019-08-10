ENGINE_API void
updateProjection(Camera* pCamera) {
    switch (pCamera->projectionType) {
        case ORTHOGRAPHIC: {
            pCamera->projection = HMM_Orthographic(-pCamera->halfCamWidth, 
                                                   pCamera->halfCamWidth, 
                                                   -pCamera->halfCamHeight, 
                                                   pCamera->halfCamHeight, 
                                                   0.f, 100.f);
        }break;
        case PERSPECTIVE: {
            pCamera->projection = HMM_Perspective(pCamera->fov,
                                                  pCamera->aspectRatio,
                                                  0.f, 100.f); 
        }break;
        default: {
            Assert(0);
        }break;
    }
}

ENGINE_API void 
initCamera(Camera* pCamera, hmm_vec3 position, hmm_vec3 up, f32 yaw, f32 pitch) {
    
    pCamera->front = -1.0f*VEC3_Z;
	pCamera->up = VEC3_Y;
	pCamera->right = VEC3_X;
	pCamera->position = position;
	pCamera->worldUp = HMM_Vec3(0.f, 1.f, 0.f);
    pCamera->yaw = 0.f;
	pCamera->pitch = 0.f;
	pCamera->roll = 0.f;
    //pCamera->rotor = rotorFromAngleAndBivec(pitch, BivecYZ);
    //pCamera->rotor *= rotorFromAngleAndBivec(yaw, BivecZX);
    pCamera->rotorYZ = RotorYZ;
    pCamera->rotorZX = RotorZX;
    pCamera->rotorXY = RotorXY;
    pCamera->movementSpeed = SPEED;
    pCamera->mouseSensitivity = SENSITIVITY;
    pCamera->zoom = ZOOM;
    pCamera->halfCamHeight = 10.f; 
    pCamera->halfCamWidth = 10.f; 
    pCamera->fov = 100;
    pCamera->aspectRatio = 800/600;
    pCamera->projectionType = PERSPECTIVE;
    pCamera->model = HMM_Mat4d(1.f);
    pCamera->firstMouseMovement = true;
    updateCameraVectors(pCamera);
    updateProjection(pCamera);
}

ENGINE_API hmm_mat4 
getViewMatrix(Camera* pCamera) {
    return HMM_LookAt(pCamera->position, 
                      pCamera->position + pCamera->front, 
                      pCamera->up);
    //hmm_mat4 result = getRotorMat4(pCamera->rotor);
    
    //return result;
}

ENGINE_API void 
moveCamera(Camera* pCamera, hmm_vec3 direction, f32 deltaTime) {
    f32 velocity = pCamera->movementSpeed * deltaTime;
    pCamera->position += direction * velocity;
}

ENGINE_API void
cameraMouseMovement(Camera* pCamera, f64 xPos, f64 yPos, b32 constrainPitch) {
    if (pCamera->firstMouseMovement) {
        pCamera->lastMousePos.x = (f32)xPos;
        pCamera->lastMousePos.y = (f32)yPos;
        pCamera->firstMouseMovement = false;
    }
    
    f32 xOffset = (f32)xPos - pCamera->lastMousePos.x;
    f32 yOffset = pCamera->lastMousePos.y - (f32)yPos; 
    pCamera->lastMousePos.x = (f32)xPos;
    pCamera->lastMousePos.y = (f32)yPos;
    
    xOffset *= pCamera->mouseSensitivity;
    yOffset *= pCamera->mouseSensitivity;
    
    pCamera->yaw += xOffset;
    pCamera->pitch += yOffset;
    
    if (constrainPitch) {
        if (pCamera->pitch > 89.0f) {
            pCamera->pitch = 89.0f;
        } if (pCamera->pitch < -89.0f) {
            pCamera->pitch = -89.0f;
        }
    }
    
    updateCameraVectors(pCamera);
}

ENGINE_API void
cameraMouseScroll(Camera* pCamera, f32 yoffset) {
    if (pCamera->zoom >= 1.0f && pCamera->zoom <= 45.0f) {
        pCamera->zoom -= yoffset;
    }
    if (pCamera->zoom <= 1.0f) {
        pCamera->zoom = 1.0f;
    }
    if (pCamera->zoom >= 45.0f) {
        pCamera->zoom = 45.0f;
    }
}

ENGINE_API inline Rotor3
rotateYZ(f32 angle) {
    f32 halfAngle = angle*0.5f;
    f32 sinAngle = sinf(halfAngle);
    f32 cosAngle = cosf(halfAngle);
    
    Rotor3 result = {};
    
    result.a = cosAngle;
    result.yz = sinAngle;
    
    return result;
}

ENGINE_API inline Rotor3
rotateZX(f32 angle) {
    f32 halfAngle = angle*0.5f;
    f32 sinAngle = sinf(halfAngle);
    f32 cosAngle = cosf(halfAngle);
    
    Rotor3 result = {};
    
    result.a = cosAngle;
    result.zx = sinAngle;
    
    return result;
}

ENGINE_API inline Rotor3
rotateXY(f32 angle) {
    f32 halfAngle = angle*0.5f;
    f32 sinAngle = sinf(halfAngle);
    f32 cosAngle = cosf(halfAngle);
    
    Rotor3 result = {};
    
    result.a = cosAngle;
    result.xy = sinAngle;
    
    return result;
}

ENGINE_API void
updateCameraVectors(Camera* pCamera) {
#if 0
    pCamera->rotorYZ = getRotatedRotor(rotorFromAngleAndBivec(pCamera->pitch*PI32/180.f, pCamera->rotorYZ.bivec), pCamera->rotorYZ);
    rotorNormalize(&pCamera->rotorYZ);
    pCamera->rotorZX = getRotatedRotor(rotorFromAngleAndBivec(pCamera->yaw*PI32/180.f, pCamera->rotorZX.bivec), pCamera->rotorZX);
    rotorNormalize(&pCamera->rotorZX);
    pCamera->rotorXY = getRotatedRotor(rotorFromAngleAndBivec(pCamera->roll*PI32/180.f, pCamera->rotorXY.bivec), pCamera->rotorXY);
    rotorNormalize(&pCamera->rotorXY);
    
    rotor = rotateXY(pCamera->roll);
    rotor *= rotateZX(pCamera->yaw);
    rotor *= rotateYZ(pCamera->pitch);
    pCamera->rotor *= rotor;
#endif
    
    hmm_vec3 front;
    front.x = cosf(HMM_ToRadians((f32)pCamera->yaw))*cosf(HMM_ToRadians((f32)pCamera->pitch));
    front.y = sinf(HMM_ToRadians((f32)pCamera->pitch));
    front.z = sinf(HMM_ToRadians((f32)pCamera->yaw))*cosf(HMM_ToRadians((f32)pCamera->pitch));
    
    pCamera->front = HMM_NormalizeVec3(front);
    pCamera->right = HMM_Cross(pCamera->front, pCamera->worldUp);
    pCamera->up = HMM_NormalizeVec3(HMM_Cross(pCamera->right, pCamera->front));
}
