
/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_UTILS
#define UNGINE_UTILS

/*────────────────────────────────────────────────────────────────────────────*/

/*
namespace ungine { namespace rl {
rl::RayCollision GetRayCollisionMesh( Ray ray, Mesh mesh, Matrix transform ){
rl::RayCollision collision = { 0 };

    // Check if mesh vertex data on CPU for testing
    if( mesh.vertices != NULL ) {
        int triangleCount = mesh.triangleCount;

        // Test against all triangles in mesh
    for( int i = 0; i < triangleCount; i++ ){
        Vector3 a, b, c;
        Vector3 *vertdata = (Vector3 *)mesh.vertices;

        if( mesh.indices){
            a = vertdata[mesh.indices[i*3 + 0]];
            b = vertdata[mesh.indices[i*3 + 1]];
            c = vertdata[mesh.indices[i*3 + 2]];
        } else {
            a = vertdata[i*3 + 0];
            b = vertdata[i*3 + 1];
            c = vertdata[i*3 + 2];
        }

        a = Vector3Transform(a, transform);
        b = Vector3Transform(b, transform);
        c = Vector3Transform(c, transform);

        RayCollision triHitInfo = GetRayCollisionTriangle(ray, a, b, c);

        if( triHitInfo.hit ){
            // Save the closest hit triangle
            if ((!collision.hit) || (collision.distance > triHitInfo.distance)) collision = triHitInfo;
        }
    
    }}

    return collision;
}}}
*/

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace rl {
Matrix GetTransformMatrix( vec3_t position, vec3_t scale, vec3_t rotation ) {
Matrix mat = {0};
    
    // Initialize Vectors
    vec4_t q_rotation = QuaternionFromEuler( 
           rotation.x, rotation.y, rotation.z
    );

    //Initialize Vectors
    Vector3 right   = { 1, 0, 0 };
    Vector3 up      = { 0, 1, 0 };
    Vector3 forward = { 0, 0, 1 };

    //Scale Vectors
    right   = Vector3Scale( right   , scale.x  );
    up      = Vector3Scale( up      , scale.y  );
    forward = Vector3Scale( forward , scale.z  );

    //Rotate Vectors
    right   = Vector3RotateByQuaternion( right  , q_rotation );
    up      = Vector3RotateByQuaternion( up     , q_rotation );
    forward = Vector3RotateByQuaternion( forward, q_rotation );
    
    // Set matrix output
	Matrix matrix_out = {
		right.x, up.x, forward.x, position.x,
		right.y, up.y, forward.y, position.y,
		right.z, up.z, forward.z, position.z,
		      0,    0,         0,          1
	};

return matrix_out; }}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { namespace rl { void BeginMode3D( camera_3D_t& cam ){
    
    rlDrawRenderBatchActive();        // Update and draw internal render batch
    rlMatrixMode(RL_PROJECTION);      // Switch to projection matrix
    rlPushMatrix(); rlLoadIdentity(); // Reset current matrix (projection)

    float aspect = GetRenderWidth() * 1.016f / GetRenderHeight();

    if ( cam.projection == CAMERA_PERSPECTIVE ) {

        double top   = cam.near * tan(cam.fovy * 0.5 * DEG2RAD);
        double right = aspect   * top;
        rlFrustum(-right, right, -top, top, cam.near, cam.far);

    } else {

        double top   = cam.fovy/2.0;
        double right = aspect * top;
        rlOrtho(-right, right, -top, top, cam.near, cam.far);

    }

    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();               // Reset current matrix (modelview)

    Matrix matView = MatrixLookAt(cam.position, cam.target, cam.up);
    rlMultMatrixf(MatrixToFloat(matView));

    rlEnableDepthTest(); // Enable DEPTH_TEST for 3D

}}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
