/*
 * Copyright 2023 The Nodepp Project Authors. All Rights Reserved.
 *
 * Licensed under the MIT (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://github.com/NodeppOfficial/nodepp/blob/main/LICENSE
 */

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_XML
#define NODEPP_XML

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/json.h>

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class xml_t {
private:

    using QUEUE = map_t<string_t,object_t>;
    using ARRAY = array_t<object_t>;

protected:

    object_t parse_tag( const string_t& str, ulong start, ulong end ) const {
        static regex_t reg( "^<([^ >]+)|(\"[^\"]*\")|(\\w+)" );
        auto raw = str.slice( start, end ); /*---------------*/
        if(!regex::test( raw,"^<(/|\\w)" ) ){ return nullptr; }
        if(!regex::test( raw,"^<|>$" ) ) /*------------------*/
          { throw except_t( "invalid xml format" ); }

        /*-------*/ reg.search_all( raw );
        auto attr = map_t<string_t,string_t>();
        auto data = reg.get_memory(); reg.clear_memory(); 

        for( auto &x: data.slice(1) ){
        if ( x[0] != '"' ){ attr[x]=string::null(); continue; }
        if ( attr.raw().empty() ) /*-----------*/ { continue; }
        auto data = regex::replace_all(x.slice(1,-1),"[\n\t\r ]+"," ");
        attr.raw().last()->data.second = data.empty()?string::null():data; }

        return object_t({
            { "type", data[0] /*-------*/ },
            { "attr", json::parse( attr ) },
        });

    }

    ARRAY parse_child( const string_t& str, const ARRAY& list ) const {

        if( str .empty() )/*----------------------*/{ return nullptr; }
        if( list.empty() )/*----------------------*/{ return nullptr; }
        if( list[0]["type"].as<string_t>()[0]=='/' ){ return nullptr; } 

        auto out = ARRAY(); /*------------------------*/ uint k=0, j=0;

        for( auto x=0; x<list.size(); x++ ){ auto a = list[x]["type"].as<string_t>(); k=0;
        for( auto y=x; y<list.size(); y++ ){ auto b = list[y]["type"].as<string_t>(); ++j;

        if  ( b[0]=='/' && b.slice(1)==a ){ --k; }
        elif( /*--------*/ b /*----*/==a ){ ++k; }
        if  ( k==0 ) /*--------------*/ { break; }

        } if( k!=0 ) { out.push( list[x] ); } else { do {
            
            auto n_item = list[x];
            auto n_list = list.slice( x+1, j-1 );
            auto n_out  = parse_child( str, n_list );
            n_item["children"]=n_out; out.push( n_item );
            
        } while(0); x=j-1; } j=x+1; }

    return out; }

    ARRAY parse_list( const string_t& str ) const {

        static regex_t reg ( "<!--([^-]+-)+->|<[^<>]+>" );
        auto data = reg.search_all( str ); reg.clear_memory();
        auto out  = ARRAY(); ulong off =0; uchar b=0;

        if ( data.empty() ){ return nullptr; }

        for( auto &x: data ){
             auto y = parse_tag ( str, x[0], x[1] );
        if (!y.has_value() ){ off=x[1]; continue; }

        if  ( regex::test( y["type"].as<string_t>(), "/script", true ) ){ b=0; }
        elif( regex::test( y["type"].as<string_t>(),  "script", true ) ){ b=1; }
        if  ( b ) /*-----------------------------------*/ { off=x[1];continue; } 
        
             auto z    = ptr_t<ulong>({ off, x[0] });
             auto data = regex::replace_all( str.slice( z[0], z[1] ), "[\r\n\t ]+", " " );

        if ( !data.empty() && !regex::test( data, "^[\r\n\t ]+$" ) )
           { out.push(object_t({ { "type", "_text_" }, { "data", data } })); }
        
        out.push(y); off=x[1]; }
        
    return out; }

    /*─······································································─*/

    string_t format_tag( const object_t& obj ) const {
    if( !obj.has_value() ){ throw except_t( "invalid tag" ); }
    if( !obj.has("type") ){ throw except_t( "invalid tag" ); }

        string_t type= obj["type"].as<string_t>();

    if( type=="_text_" ){ return obj["data"].as<string_t>(); }

        string_t out = string::format( "<%s", type.get() );
        string_t end = string::format( ">${0}</%s>", type.get() ); 

        for( auto x: obj["attr"].keys() ){ 
             auto y= json::stringify ( obj["attr"][x] ); 
             out += regex::format( " ${0}=${1}", x, y ); 
        }

        if ( !obj.has("children") )
             { return out + "/>"; }
        else { return out + end ; }

    }

    string_t format_object( const object_t& obj ) const {
    if( !obj.has_value() ){ return string::null(); }
        
        string_t tag = format_tag( obj );

        if( obj.has( "children" ) ){
            string_t borrow= format_array( obj["children"].as<ARRAY>() );
            return regex::format( tag, borrow );
        } else { return tag; } 

    }

    string_t format_array( const ARRAY& obj ) const { 
    if( obj.empty() ){ return string::null(); }
    string_t borrow; for( auto x: obj ){
    /*----*/ borrow += format_object( x );
    } return borrow; }

public:

    /*----*/ xml_t() noexcept {}
    virtual ~xml_t() noexcept {}

    ARRAY parse( const string_t& str ) const {
        return parse_child( str, parse_list(str) );
    }

    string_t format( const ARRAY& obj ) const { try {
        return format_array( obj );
    } catch( except_t ) {
        throw except_t( "invalid xml object" );
    }}

    string_t format( const object_t& obj ) const { try {
        return format_object( obj );
    } catch( except_t ) {
        throw except_t( "invalid xml object" );
    }}
        
};}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    array_t<object_t> parse( const string_t& data ){ 
        return xml_t().parse(data); 
    }

    string_t format( const array_t<object_t>& data ){ 
        return xml_t().format(data); 
    }

    string_t format( const object_t& data ){ 
        return xml_t().format(data); 
    }

} }

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    inline void has( array_t<object_t> dom, string_t tag, function_t<void,object_t> callback ){
        for( auto x: dom ){ if( x.has("children") ){
        for( auto y: x["children"].as<decltype(dom)>() ){
        if ( y["type"].as<string_t>() == tag ){ callback(x); break; } 
        } has( x["children"].as<decltype(dom)>(), tag, callback ); }}
    }

    inline void every( array_t<object_t> dom, function_t<void,object_t> callback ){
        for( auto x: dom ){ callback(x); if( x.has("children") ){
             every( x["children"].as<decltype(dom)>(),callback );
        }}
    }

    inline void some( array_t<object_t> dom, function_t<void,object_t> callback ){
        for( auto x: dom ){ callback(x); }
    }

    inline bool check( string_t type, object_t& dom ){ try {

        static regex_t reg ( "[^\\[\\^\\|\\$\\]=>*\"\']+" );
            
        if( !dom.has_value() ) /*--------------*/ { return false; }
        if( !dom.has( "type" )) /*-------------*/ { return false; }
        if( dom["type"].as<string_t>()=="_text_" ){ return false; }
        
        if( type.empty() ) /*------------------*/ { return true ; }
        if( type[0]=='*' ) /*------------------*/ { return true ; }

        if( type[0] == '[' ){ auto data = reg.match_all( type );
        if( data.empty() || !dom.has("attr") )   { return false; }
        if( data.size()==1 ){ return dom["attr"].has( data[0] ); }
        if( !dom["attr"].has( data[0] ) ) /*--*/ { return false; }
            
            if  ( regex::test( type, "\\*=" ) )
                { return regex::test( dom["attr"][data[0]].as<string_t>(), data[1], true ); }

            elif( regex::test( type, "\\$=" ) )
                { return regex::test( dom["attr"][data[0]].as<string_t>(), data[1]+"$", true ); }

            elif( regex::test( type, "\\^=" ) )
                { return regex::test( dom["attr"][data[0]].as<string_t>(), "^"+data[1], true ); }
            
            elif( regex::test( type, "=" ) )
                { return regex::test( dom["attr"][data[0]].as<string_t>(), "^"+data[1]+"$", true ); }

        } else {
            return dom["type"].as<string_t>()==type;
        }

    } catch( except_t ) {} return false; };

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    array_t<object_t> find_element( array_t<object_t> node, string_t pttr ){
        if( node.empty() ){ return nullptr; } queue_t<object_t> out;

    /*─······································································─*/

        static function_t<decltype(out),string_t,const decltype(out)&> select_by_type(
        [&]( string_t tag, const decltype(out)& dom ){ try {

            queue_t<object_t> /*----------*/ out; 
            every( dom.data(), [&]( object_t x ){
            if   ( check( tag, x ) ){ out.push(x); }});

        return out; } catch( except_t ) {} return decltype(out)( nullptr ); });

    /*─······································································─*/

        static function_t<decltype(out),bool,string_t,string_t,const decltype(out)&> select_by_chld(
        [&]( bool mode, string_t tag_a, string_t tag_b, const decltype(out)& dom ){ try {

            queue_t<object_t> /*-------------*/ out;
            auto tmp = select_by_type( tag_a, dom );

            if( mode == 0 ){

                some( tmp.data(), [&]( object_t x ){
                if  ( check( tag_b, x ) ){ out.push(x); }});

            } else {

                every( tmp.data(), [&]( object_t x ){
                if   ( check( tag_b, x ) ){ out.push(x); }});

            }
                
        return out; } catch( except_t ) {} return decltype(out)( nullptr ); });

    /*─······································································─*/

        static function_t<decltype(out),bool,string_t,string_t,const decltype(out)&> select_by_sibl(
        [&]( bool mode, string_t tag_a, string_t tag_b, const decltype(out)& dom ){ try {

            queue_t<object_t> /*-----------------------------*/ out;
            has( dom.data(), tag_a, [&]( object_t x ){ bool w=false;

                if( mode == 0 ){ 

                    for( auto z: x["children"].as<array_t<object_t>>() ){
                    if ( z["type"].as<string_t>()==  "_text_"  ){ continue; /*--------*/ }
                    if ( z["type"].as<string_t>()==tag_a && !w ){ w=true  ; /*--------*/ }
                    if ( z["type"].as<string_t>()==tag_b &&  w ){ w=false ; out.push(x); }}

                } else {

                    some( x["children"].as<array_t<object_t>>(), [&]( object_t x ){
                    if  ( check( tag_b, x ) ){ out.push(x); }});

                }

            });

        return out; } catch( except_t ) {} return decltype(out)( nullptr ); });

    /*─······································································─*/

        static function_t<decltype(out),string_t*,ulong*,ulong,const decltype(out)&> query_selector(
        [&]( string_t* addr, ulong* offset, ulong size, const decltype(out)& dom ){ try { 
            
        if ( *offset >= size )/*----------*/{ throw except_t("invalid query"); }
        if ( size-*offset<=2 ){ return select_by_type( *(addr+*offset), dom ); }

            queue_t<object_t> /**/ out;
            string_t a= * ( addr + 0 );
            string_t b= * ( addr + 1 );
            string_t c= * ( addr + 2 );

            if  ( regex::test( b, "+" ) ){ out = select_by_sibl( 0, a, c, dom ); *offset = *offset + 1; }
            elif( regex::test( b, "~" ) ){ out = select_by_sibl( 1, a, c, dom ); *offset = *offset + 1; }
            elif( regex::test( b, ">" ) ){ out = select_by_chld( 0, a, c, dom ); *offset = *offset + 1; }
            elif( regex::test( b, " " ) ){ out = select_by_chld( 1, a, c, dom ); *offset = *offset + 1; }
            else /*-------------------*/ { out = select_by_type( a, dom ); /*-*/ *offset = *offset + 0; }
        
        return out; } catch( except_t ) {} return decltype(out)( nullptr ); });

    /*─······································································─*/

        if( !regex::test( pttr, "," ) ){
        
        auto  patt= "\\[[^\\]]+\\]|[^\\[ ~+>]+|([ ~+>]?)+";
        auto  regs= regex::match_all( pttr, patt ); 
        ulong offs= 0; /*------------------------*/

        for  ( auto &x: node )/**/{ out.push(x); }
        while( offs<regs.size() && !out.empty() ){

            out = query_selector( regs.begin(), &offs, regs.size(), out );

        ++offs; }} else {

            for( auto &x: regex::split( pttr, "," ) ){
            for( auto &y: find_element( node, x   ) ){ out.push( y ); }}

        }
    
    return out.data(); }

    array_t<object_t> find_element( object_t element, string_t pttr ){
        if( !element.has_value() ) /*---------------------*/ { return nullptr; }
        if( !element.has( "children" ) ) /*---------------*/ { return nullptr; }
        return find_element( element["children"].as<array_t<object_t>>(), pttr );
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace xml {

    string_t get_xml( array_t<object_t> dom ) { return format( dom ); }
    string_t get_xml( object_t dom ) /*----*/ { return format( dom ); }

    void set_attribute( object_t obj, string_t name, string_t value ) {
         obj["attr"][ name ] = value;
    }

    bool has_attribute( object_t obj, string_t name ) {
         return obj["attr"].has( name );
    }
    
    object_t append_child( object_t& parent, object_t child ){
        if(!parent.has_value() ){ return /*--*/ nullptr; }
        if( parent.has( "children" ) ){
            parent["children"].as<array_t<object_t>>()
            /*--------------*/.push( child );
        } else {
            auto array = array_t<object_t>();
            /**/ array.push( child );
            parent["children"]=array;
        }   return parent;
    }

    object_t create_element( string_t type ) { return object_t({
        { "type", type }, { "attr", object_t() }
    }); }

    string_t get_attribute( object_t obj, string_t name ) {
         return obj["attr"][ name ].as<string_t>();
    }

    string_t get_text( array_t<object_t> dom ) {
        string_t out; every( dom, [&]( object_t dom ){
            if( dom["type"].as<string_t>()=="_text_" ) 
              { out += dom["data"].as<string_t>() +" "; }
        }); return out;
    }

    string_t get_text( object_t element ) {
        if( !element.has( "children" ) ) /*-----*/ { return nullptr; }
        return get_text( element["children"].as<array_t<object_t>>() );
    }

} }

/*────────────────────────────────────────────────────────────────────────────*/

#endif

/*────────────────────────────────────────────────────────────────────────────*/
