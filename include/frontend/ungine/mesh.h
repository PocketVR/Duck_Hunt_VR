/*
 * Copyright 2023 The Ungine Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/UngineOfficial/Ungine/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef UNGINE_MESH
#define UNGINE_MESH

/*────────────────────────────────────────────────────────────────────────────*/

namespace ungine { class mesh_t : public global_t {
protected:

    struct NODE { rl::Mesh ctx={0}; };  ptr_t<NODE> obj;

public:

    /*─······································································─*/

    mesh_t( rl::Mesh mesh ) noexcept : global_t(), obj( new NODE() ){ obj->ctx = mesh; }

    virtual ~mesh_t() noexcept { if( obj.count()>1 ){ return; } free(); }

    /*----*/ mesh_t() noexcept : global_t(), obj( new NODE() ){ }

    /*─······································································─*/

    bool is_valid() const noexcept { return obj->ctx.vertexCount == 0; }

    rl::Mesh* operator->() const noexcept { return &get(); }

    rl::Mesh& get() const noexcept { return obj->ctx; }

    /*─······································································─*/

    /*
    void draw( transform_3D_t pos, material_t material ){}
    */

    /*─······································································─*/

    void free() const noexcept { 
         if( obj->ctx.vertexCount==0 ){ return; } 
         rl::UnloadMesh( obj->ctx ); 
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

#endif