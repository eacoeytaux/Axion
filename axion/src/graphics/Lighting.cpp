#include "Lighting.hpp"

#include "Camera.hpp"

AXN_NAMESPACES
using axn::graphics::Lighting;

Lighting::Lighting( ) : m_light_sources( { } ) { m_darkness_slider.value( 0 ); }

const Lighting & Lighting::render( Camera* camera ) const {
    Visible::render( );
    Shape capture = Shape::rectangle( camera->width( ), camera->height( ), camera->center( ) );
    //render_lights( capture );
    //render_darkness( capture );
    return *this;
}

//const Lighting & Lighting::render_lights( const RectangleFixed & _capture ) const {
//    for_each( light, m_light_sources ) {
//        VectorA offset( Random::r_Angle( ), Random::rng_float( light.flicker( ) ) );
//        draw( light.tint( ), Shape::circle( light.distance( ), light.position( ) + offset ) );
//    }
//    return *this;
//}
//
//const Lighting & Lighting::render_darkness( const Shape & _capture ) const {
//    // if (darkness_active() && m_darkness_slider.value()) {
//    //     Rectangle full(_capture.width(), _capture.height(),
//    //     _capture.center()); fill_boundary_recursive(full, m_light_sources);
//    // }
//    return *this;
//}

bool Lighting::darkness_active( ) const { return m_darkness_active; }

Lighting & Lighting::darkness_active( const bool _darkness_active ) {
    m_darkness_active = _darkness_active;
    return *this;
}

ufloat Lighting::darkness_intensity( ) const { return m_darkness_slider.value( ); }

Lighting & Lighting::darkness_intensity( const ufloat _intensity ) {
    m_darkness_slider.value( _intensity );
    return *this;
}

Lighting & Lighting::add_light_source( const LightSource & _light_source ) {
    m_light_sources.push_back( _light_source );
    return *this;
}

// Lighting & Lighting::add_light_sources( const varray<LightSource> &
// _light_source) {
//    m_light_sources.add_varray(_light_source);
//     return *this;
// }

Lighting & Lighting::clear_light_sources( ) {
    m_light_sources.clear( );
    return *this;
}

//const Lighting & Lighting::fill_boundary_dark( const RectangleFixed & _boundary ) const {
//    draw( Color( BLACK ).a( m_darkness_slider.value( ) * 255 ), _boundary );
//    return *this;
//}
//
//const Lighting & Lighting::fill_boundary_clear( const RectangleFixed & _boundary,
//                                                const LightSource & source ) const {
//    draw( source.tint( ), _boundary );
//    draw( Color( WHITE ).a( m_darkness_slider.value( ) * 16 ), _boundary );
//    return *this;
//}
//
//const Lighting & Lighting::fill_boundary_edge( const RectangleFixed & _boundary,
//                                               const LightSource & _source,
//                                               const Quadrant & _quadrant ) const {
//    Vector v1;
//    Vector v2;
//    if( ( _quadrant == Q1 ) || ( _quadrant == Q3 ) ) {
//        v1 = Vector( _source.position( ), _boundary.top_left( ) );
//        v2 = Vector( _source.position( ), _boundary.bottom_right( ) );
//    } else if( ( _quadrant == Q2 ) || ( _quadrant == Q4 ) ) {
//        v1 = Vector( _source.position( ), _boundary.top_right( ) );
//        v2 = Vector( _source.position( ), _boundary.bottom_left( ) );
//    }
//
//    varray<Line> arc =
//        Circle( _source.distance( ), _source.position( ) ).arc( v1.angle( ), v2.angle( ) );
//    //    draw(CYAN, arc );
//
//    draw( Color( WHITE ).a( 127 ), Line( v1.destination( ), v2.destination( ) ) );
//    //    draw(_source.tint(), _boundary);
//    //    draw(RED, _boundary, 2);
//
//    return *this;
//}
//
//const Lighting & Lighting::fill_boundary_recursive(
//    const RectangleFixed & _rectangle, const varray<LightSource> & _possible_sources,
//    const varray<LightSource> & _sources ) const {
//    if( !_rectangle.area( ) )
//        return *this;
//
//    // check for new sources
//    bool found_new_light = false;
//    varray<LightSource> new_sources = _sources;
//    varray<LightSource> new_possible_sources = _possible_sources;
//    if( _possible_sources.size( ) ) {
//        LightSource source = _possible_sources[ 0 ];
//
//        for_range( _possible_sources.size( ) ) {
//            LightSource temp_source = _possible_sources[ i ];
//            Rectangle boundary = _rectangle;
//            // expand to include radius instead of just center
//            boundary.width( boundary.width( ) + ( temp_source.distance( ) * 2.f ) );
//            boundary.height( boundary.height( ) + ( temp_source.distance( ) * 2.f ) );
//            if( boundary.contains( temp_source.position( ) ) ) {
//                found_new_light = true;
//                source = temp_source;
//                new_possible_sources.remove( i );
//                break;
//            }
//        }
//
//        if( found_new_light ) {
//            // found new light source, recurse further
//
//            new_sources.push_back( source );
//            Circle light_circle( source.distance( ),
//                                 source.position( ) ); // TODO unused?
//            RectangleFixed light_boundary( source.distance( ) * 2.f, source.distance( ) * 2.f,
//                                           source.position( ) );
//            RectangleFixed::SplitFixed split_rectangles =
//                _rectangle.split( source.position( ) );
//
//            // 5|3|4
//            // 5|3|4
//            // 5|-|4
//            // 5|1|4
//            // 5|-|4
//            // 5|2|4
//            // 5|2|4
//
//            RectangleFixed left;
//            RectangleFixed right;
//            RectangleFixed center_top;
//            RectangleFixed center_bottom;
//            RectangleFixed center_w_light;
//
//            RectangleFixed::SplitFixed left_split = _rectangle.split(
//                Coordinate( source.position( ).x( ) - source.distance( ), NEGATIVE_INFINITY ) );
//            left = left_split.top_left( );
//            RectangleFixed::SplitFixed right_split = left_split.top_right( ).split(
//                Coordinate( source.position( ).x( ) + source.distance( ), NEGATIVE_INFINITY ) );
//            right = right_split.top_right( );
//            RectangleFixed::SplitFixed center_top_split = right_split.top_left( ).split(
//                Coordinate( NEGATIVE_INFINITY, source.position( ).y( ) + source.distance( ) ) );
//            center_top = center_top_split.top_right( );
//            RectangleFixed::SplitFixed center_bottom_split =
//                center_top_split.bottom_right( ).split( Coordinate(
//                    NEGATIVE_INFINITY, source.position( ).y( ) - source.distance( ) ) );
//            center_bottom = center_bottom_split.bottom_right( );
//            center_w_light = center_bottom_split.top_right( );
//
//            // TODO update to sort by x
//
//            fill_boundary_recursive( left, new_possible_sources, _sources );
//            fill_boundary_recursive( right, new_possible_sources, _sources );
//            fill_boundary_recursive( center_top, new_possible_sources, _sources );
//            fill_boundary_recursive( center_bottom, new_possible_sources, _sources );
//
//            fill_boundary_recursive( center_w_light, new_possible_sources, new_sources );
//        }
//    }
//
//    // no new lights, can stop recursing
//    if( !found_new_light ) {
//        if( !new_sources.size( ) ) { // base case
//            // no sources, is fully dark
//            fill_boundary_dark( _rectangle );
//        } else {
//            if( new_sources.size( ) == 1 ) {
//                LightSource source = new_sources[ 0 ];
//                fill_boundary_clear( _rectangle, source );
//
//                bool immersed = true;
//                bool excluded = true;
//                for_each( coordinate, _rectangle.coordinates( ) ) {
//                    if( source.position( ).distance( coordinate ) > source.distance( ) ) {
//                        immersed = false;
//                    } else {
//                        excluded = false;
//                    }
//                }
//
//                if( immersed ) {
//                    // fill_boundary_clear(drawing, _rectangle, lighting);
//                } else if( excluded ) {
//                    fill_boundary_dark( _rectangle );
//                } else {
//
//                    // there must be 2 intersection points
//                    // if split where these points intersection going
//                    // perpendicular from their edges you'll get: 1 edge corner
//                    // boundary where it connects corner to corner 3 either all
//                    // filled or all dark boundaries
//
//                    RectangleFixed::SplitFixed light_split =
//                        _rectangle.split( source.position( ) );
//
//                    // fill_boundary_edge(drawing, light_split.top_right(),
//                    // lighting, Q1); fill_boundary_edge(drawing,
//                    // light_split.top_left(), lighting, Q2);
//                    // fill_boundary_edge(drawing, light_split.bottom_left(),
//                    // lighting, Q3); fill_boundary_edge(drawing,
//                    // light_split.bottom_right(), lighting, Q4);
//                }
//            } else {
//                // TODO
//                // lighting = new_sources[0];
//                // fill_boundary_clear(drawing, _rectangle, lighting);
//            }
//        }
//    }
//
//    //    if (!matched.size()) {
//    //        if (_sources.size() == 1) {
//    //        }
//    //    }
//    //
//    //    if (_sources.size() == 1) {
//    //
//    //    } else {
//    //
//    //    }
//    //}
//    //
//    // return;
//    //
//    // if (false) { } else {
//    //    // check for new sources
//    //    for_range (_possible_sources.size()) {
//    //        LightSource light = _possible_sources[i];
//    //        Rectangle boundary = _rectangle;
//    //        // expand to include radius instead of just center
//    //        boundary.width(boundary.width() + (light.distance * 2));
//    //        boundary.height(boundary.height() + (light.distance * 2));
//    //        if (boundary.contains(light.position())) {
//    //
//    //            // found new light source, recurse further
//    //
//    //            Circle light_circle(light.distance, light.position()); // TODO
//    //            unused? Rectangle light_boundary(light.distance * 2,
//    //            light.distance * 2, light.position());
//    //            Rectangle::RectangleSplit split_rectangles =
//    //            _rectangle.split(light.position());
//    //
//    //            // 5|3|4
//    //            // 5|3|4
//    //            // 5|-|4
//    //            // 5|1|4
//    //            // 5|-|4
//    //            // 5|2|4
//    //            // 5|2|4
//    //
//    //            Rectangle left;
//    //            Rectangle right;
//    //            Rectangle center_top;
//    //            Rectangle center_bottom;
//    //            Rectangle center_w_light;
//    //
//    //            Rectangle::RectangleSplit left_split =
//    //            _rectangle.split(Coordinate(light.position().x() -
//    //            light.distance, NEGATIVE_INFINITY)); left =
//    //            left_split.top_left(); Rectangle::RectangleSplit right_split =
//    //            left_split.top_right().split(Coordinate(light.position().x() +
//    //            light.distance, NEGATIVE_INFINITY)); right =
//    //            right_split.top_right(); Rectangle::RectangleSplit
//    //            center_top_split =
//    //            right_split.top_left().split(Coordinate(NEGATIVE_INFINITY,
//    //            light.position().y() + light.distance)); center_top =
//    //            center_top_split.top_right(); Rectangle::RectangleSplit
//    //            center_bottom_split =
//    //            center_top_split.bottom_right().split(Coordinate(NEGATIVE_INFINITY,
//    //            light.position().y() - light.distance)); center_bottom =
//    //            center_bottom_split.bottom_right(); center_w_light =
//    //            center_bottom_split.top_right();
//    //
//    //            // TODO update to sort by x
//    //            varray<LightSource> new_possible_sources = _possible_sources;
//    //            new_possible_sources.erase(new_possible_sources.begin() + i);
//    //
//    //            draw_recursive(_camera, left, new_possible_sources, { });
//    //            draw_recursive(_camera, right, new_possible_sources, { });
//    //            draw_recursive(_camera, center_top, new_possible_sources, {
//    //            }); draw_recursive(_camera, center_bottom,
//    //            new_possible_sources, { });
//    //
//    //            draw_recursive(_camera, center_w_light, new_possible_sources,
//    //            { light });
//    //        }
//    //    }
//    //}
//    //
//    // return;
//    //
//    // varray<LightSource> updated_sources = _sources;
//    // for_each (source, _sources) {
//    //    //if (source.position().distance(_rectangle.center()) <
//    //    source.distance) {
//    //    //    updated_sources.push_back(source);
//    //    //    continue;
//    //    //}
//    //
//    //    Coordinate farthest, closest;
//    //    Angle d_angle(source.position(), _rectangle.center()).radians(),
//    //    true); Quadrant quadrant = d_angle.quadrant(); switch (quadrant) {
//    //        case Q1: {
//    //            farthest = _rectangle.top_right();
//    //            closest = _rectangle.bottom_left();
//    //            break;
//    //        }
//    //        case Q2: {
//    //            farthest = _rectangle.top_left();
//    //            closest = _rectangle.bottom_right();
//    //            break;
//    //        case Q3: {
//    //        }
//    //            farthest = _rectangle.bottom_left();
//    //            closest = _rectangle.top_right();
//    //            break;
//    //        case Q4: {
//    //            farthest = _rectangle.bottom_right();
//    //            closest = _rectangle.top_left();
//    //            break;
//    //        }
//    //        default:
//    //            if ((d_angle > PI_1D2) && (d_angle <= PI_3D2)) {
//    //                farthest = _rectangle.top_left();
//    //                closest = _rectangle.bottom_right();
//    //            } else {
//    //                farthest = _rectangle.top_right();
//    //                closest = _rectangle.bottom_left();
//    //            }
//    //        }
//    //    }
//    //
//    //    // section is entirely emersed in light, no need to recurse further
//    //    if (source.position().distance(farthest) + 1 < source.distance) {
//    //        _camera->draw(Color(GREEN, 32) /*light.tint()*/, _rectangle);
//    //        return;
//    //    }
//    //}
//    //
//    // check for new sources
//    // // TODO light sources could be sorted by x and searched more efficiently?
//    // for_range (_possible_sources.size()) {
//    //    LightSource light = _possible_sources[i];
//    //    Rectangle boundary = _rectangle;
//    //    // expand to include radius instead of just center
//    //    boundary.width(boundary.width() + (light.distance * 2));
//    //    boundary.height(boundary.height() + (light.distance * 2));
//    //    if (boundary.contains(light.position())) {
//    //
//    //        // found new light source, recurse further
//    //
//    //        updated_sources.push_back(light);
//    //
//    //        Circle light_circle(light.distance, light.position()); // TODO
//    //        unused? Rectangle light_boundary(light.distance * 2,
//    //        light.distance * 2, light.position()); Rectangle::RectangleSplit
//    //        split_rectangles = _rectangle.split(light.position());
//    //
//    //        // 5|3|4
//    //        // 5|3|4
//    //        // 5|-|4
//    //        // 5|1|4
//    //        // 5|-|4
//    //        // 5|2|4
//    //        // 5|2|4
//    //
//    //        Rectangle left;
//    //        Rectangle right;
//    //        Rectangle center_top;
//    //        Rectangle center_bottom;
//    //        Rectangle center_w_light;
//    //
//    //        Rectangle::RectangleSplit left_split =
//    //        _rectangle.split(Coordinate(light.position().x() - light.distance,
//    //        NEGATIVE_INFINITY)); left = left_split.top_left();
//    //        Rectangle::RectangleSplit right_split =
//    //        left_split.top_right().split(Coordinate(light.position().x() +
//    //        light.distance, NEGATIVE_INFINITY)); right =
//    //        right_split.top_right(); Rectangle::RectangleSplit
//    //        center_top_split =
//    //        right_split.top_left().split(Coordinate(NEGATIVE_INFINITY,
//    //        light.position().y() + light.distance)); center_top =
//    //        center_top_split.top_right(); Rectangle::RectangleSplit
//    //        center_bottom_split =
//    //        center_top_split.bottom_right().split(Coordinate(NEGATIVE_INFINITY,
//    //        light.position().y() - light.distance)); center_bottom =
//    //        center_bottom_split.bottom_right(); center_w_light =
//    //        center_bottom_split.top_right();
//    //
//    //
//    //        // TODO update to sort by x
//    //        varray<LightSource> new_possible_sources = _possible_sources;
//    //        new_possible_sources.erase(new_possible_sources.begin() + i);
//    //
//    //        draw_recursive(_camera, left, new_possible_sources, _sources);
//    //        draw_recursive(_camera, right, new_possible_sources, _sources);
//    //        draw_recursive(_camera, center_top, new_possible_sources,
//    //        _sources); draw_recursive(_camera, center_bottom,
//    //        new_possible_sources, _sources);
//    //
//    //        draw_recursive(_camera, center_w_light, new_possible_sources,
//    //        updated_sources);
//    //
//    //        //draw_recursive(_camera, left, m_light_sources);
//    //        //draw_recursive(_camera, right, m_light_sources);
//    //        //Rectangle rectangle = center_light;
//    //        //
//    //        //    if (rectangle.area()) {
//    //        //        varray<LightSource> split_sources;
//    //        //        for_each (possible_light, _possible_sources) {
//    //        //            if (possible_light == light) continue;
//    //        //            Rectangle boundary = rectangle;
//    //        //            boundary.width(boundary.width() +
//    //        (possible_light.distance * 2) - 1);
//    //        //            boundary.height(boundary.height() +
//    //        (possible_light.distance * 2)  - 1);
//    //        //            if (boundary.contains(possible_light.position())) {
//    //        //                split_sources.push_back(possible_light);
//    //        //            }
//    //        //        }
//    //        //        Rectangle::RectangleSplit rectangle_split =
//    //        rectangle.split(light_boundary.lines()[i].c1());
//    //        //        for_each (rectangle_split_split, rectangle_split) {
//    //        //            draw_recursive(_camera, rectangle_split_split,
//    //        split_sources, updated_sources);
//    //        //        }
//    //        //    }
//    //        //}
//    //
//    //        return;
//    //    }
//    //}
//    //
//    // if (!updated_sources.size()) {
//    //    fill_boundary_full(_camera, _rectangle);
//    //    return;
//    //}
//    //
//    // else if (updated_sources.size() == 1) {
//    //    LightSource source = updated_sources[0];
//    //    Circle subtraction(source.distance, source.position());
//    //
//    //    Angle d_angle(subtraction.center(), _rectangle.center(), true);
//    //    Quadrant quadrant = d_angle.quadrant();
//    //
//    //    Coordinate c1;
//    //    Coordinate c2;
//    //
//    //    switch (quadrant) {
//    //        default:
//    //        case Q1: {
//    //            c1 = _rectangle.bottom_right();
//    //            c2 = _rectangle.top_left();
//    //            break;
//    //        }
//    //        case Q2: {
//    //            c1 = _rectangle.bottom_left();
//    //            c2 = _rectangle.top_right();
//    //            break;
//    //        }
//    //        case Q3: {
//    //            c1 = _rectangle.top_left();
//    //            c2 = _rectangle.bottom_right();
//    //            break;
//    //        }
//    //        case Q4:
//    //            c1 = _rectangle.top_right();
//    //            c2 = _rectangle.bottom_left();
//    //            break;
//    //        }
//    //    }
//    //
//    //    ufloat x_ratio = subtraction.center().distance(c1) /
//    //    subtraction.radius(); ufloat y_ratio =
//    //    subtraction.center().distance(c2) / subtraction.radius();
//    //
//    //    bool x_ratio_greater = (x_ratio >= 1);
//    //    bool y_ratio_greater = (y_ratio >= 1);
//    //
//    //    Coordinate split_point;
//    //
//    //    if ((quadrant == Q1)) {
//    //
//    //    if (x_ratio_greater && y_ratio_greater) {
//    //
//    //        if (quadrant == Q1) {
//    //            split_point =
//    //            Coordinate(subtraction.x_high(_rectangle.lower_bound_y()),
//    //            subtraction.y_high(_rectangle.lower_bound_x()));
//    //        } else if (quadrant == Q2) {
//    //            split_point =
//    //            Coordinate(subtraction.x_low(_rectangle.lower_bound_y()),
//    //            subtraction.y_high(_rectangle.upper_bound_x()));
//    //        } else if (quadrant == Q3) {
//    //            split_point =
//    //            Coordinate(subtraction.x_low(_rectangle.upper_bound_y()),
//    //            subtraction.center().y() -
//    //            (subtraction.y_high(_rectangle.upper_bound_x()) -
//    //            subtraction.center().y()));
//    //        } else if (quadrant == Q4) {
//    //            split_point =
//    //            Coordinate(subtraction.x_low(_rectangle.upper_bound_y()),
//    //            subtraction.center().y() -
//    //            (subtraction.y_high(_rectangle.upper_bound_x()) -
//    //            subtraction.center().y()));
//    //        } else {
//    //            Assert(false);
//    //        }
//    //
//    //        Rectangle::RectangleSplit rectangle_split =
//    //        _rectangle.split(split_point);
//    //
//    //        for_range (q, QuadrantS) {
//    //            if (quadrant == Q2) { // idk why this one is special
//    //                if (q != Q2) fill_boundary_full(_camera,
//    //                rectangle_split[q]); else fill_boundary_edge(_camera,
//    //                rectangle_split[q], source, quadrant); continue;
//    //            }
//    //
//    //            if (q != ((quadrant + 2) % 4)) fill_boundary_full(_camera,
//    //            rectangle_split[q]); else fill_boundary_edge(_camera,
//    //            rectangle_split[q], source, quadrant);
//    //        }
//    //    } else if (x_ratio_greater || y_ratio_greater) {
//    //        switch (quadrant) {
//    //            case Q1: {
//    //                split_point = x_ratio_greater ?
//    //                Coordinate(subtraction.x_high(_rectangle.upper_bound_y()),
//    //                _rectangle.upper_bound_y()) :
//    //                Coordinate(_rectangle.upper_bound_x(),
//    //                subtraction.y_high(_rectangle.upper_bound_x())); break;
//    //            }
//    //            case Q2: {
//    //                split_point = x_ratio_greater ?
//    //                Coordinate(subtraction.x_low(_rectangle.upper_bound_y()),
//    //                _rectangle.upper_bound_y()) :
//    //                Coordinate(_rectangle.lower_bound_x(),
//    //                subtraction.y_high(_rectangle.lower_bound_x())); break;
//    //            }
//    //            case Q3: {
//    //                split_point = x_ratio_greater ?
//    //                Coordinate(subtraction.x_low(_rectangle.lower_bound_y()),
//    //                _rectangle.lower_bound_y()) :
//    //                Coordinate(_rectangle.lower_bound_x(),
//    //                subtraction.y_low(_rectangle.lower_bound_x())); break;
//    //            }
//    //            case Q4: {
//    //                split_point = x_ratio_greater ?
//    //                Coordinate(subtraction.x_high(_rectangle.lower_bound_y()),
//    //                _rectangle.lower_bound_y()) :
//    //                Coordinate(_rectangle.upper_bound_x(),
//    //                subtraction.y_low(_rectangle.upper_bound_x())); break;
//    //            }
//    //            default: {
//    //                Assert(false);
//    //                return;
//    //            }
//    //        }
//    //
//    //        Rectangle::RectangleSplit rectangle_split =
//    //        _rectangle.split(split_point);
//    //
//    //        Rectangle edge_boundary;
//    //        int edge_boundary_split_index = 0;
//    //        switch (quadrant) {
//    //            case Q1: {
//    //                edge_boundary_split_index = x_ratio_greater ? 3 : 1;
//    //                edge_boundary = x_ratio_greater ?
//    //                rectangle_split.bottom_right() :
//    //                rectangle_split.top_left(); break;
//    //            }
//    //            case Q2: {
//    //                edge_boundary_split_index = x_ratio_greater ? 2 : 0;
//    //                edge_boundary = x_ratio_greater ?
//    //                rectangle_split.bottom_left() :
//    //                rectangle_split.top_right(); break;
//    //            }
//    //            case Q3: {
//    //                edge_boundary_split_index = x_ratio_greater ? 1 : 3;
//    //                edge_boundary = x_ratio_greater ?
//    //                rectangle_split.top_left() :
//    //                rectangle_split.bottom_right(); break;
//    //            }
//    //            case Q4: {
//    //                edge_boundary_split_index = x_ratio_greater ? 0 : 2;
//    //                edge_boundary = x_ratio_greater ?
//    //                rectangle_split.top_right() :
//    //                rectangle_split.bottom_left(); break;
//    //            }
//    //            default: {
//    //                Assert(false);
//    //                return;
//    //            }
//    //        }
//    //        fill_boundary_edge(_camera,
//    //        rectangle_split[edge_boundary_split_index], source, quadrant);
//    //        //fill_boundary_edge(_camera, edge_boundary, subtraction,
//    //        quadrant);
//    //
//    //        for_range (q, QuadrantS) {
//    //            if (edge_boundary_split_index != q)
//    //            _camera->draw(source.tint(), rectangle_split[q]);
//    //        }
//    //    } else {
//    //        _camera->draw(Color(YELLOW, 64), _rectangle);
//    //    }
//    //
//    //    }
//    //
//    //    if (d_angle < PI_1D2) {
//    //        //ufloat x_ratio =
//    //        subtraction.center().distance(_rectangle.bottom_right()) /
//    //        subtraction.radius();
//    //        //ufloat y_ratio =
//    //        subtraction.center().distance(_rectangle.top_left()) /
//    //        subtraction.radius();
//    //        //
//    //        //if (x_ratio_greater && y_ratio_greater) {
//    //        //    Coordinate split_point =
//    //        Coordinate(subtraction.x_high(_rectangle.lower_bound_y()),
//    //        subtraction.y_high(_rectangle.lower_bound_x()));
//    //        //    Rectangle::RectangleSplit rectangle_split =
//    //        _rectangle.split(split_point);
//    //        //    fill_boundary_full(_camera, rectangle_split.top_right());
//    //        //    fill_boundary_full(_camera, rectangle_split.bottom_right());
//    //        //    fill_boundary_full(_camera, rectangle_split.top_left());
//    //        //    fill_boundary_edge(_camera, rectangle_split.bottom_left(),
//    //        subtraction, Q1);
//    //        //} else if x_ratio_greater {
//    //        //    Coordinate split_point =
//    //        Coordinate(subtraction.x_high(_rectangle.upper_bound_y()),
//    //        _rectangle.upper_bound_y());
//    //        //    Rectangle::RectangleSplit rectangle_split =
//    //        _rectangle.split(split_point);
//    //        //    fill_boundary_edge(_camera, rectangle_split.bottom_right(),
//    //        subtraction, Q1);
//    //        //    _camera->draw_rectangle(Color(GREEN, 64),
//    //        rectangle_split.top_right());
//    //        //    _camera->draw_rectangle(Color(GREEN, 64),
//    //        rectangle_split.bottom_left());
//    //        //    _camera->draw_rectangle(Color(GREEN, 64),
//    //        rectangle_split.top_left());
//    //        //} else if y_ratio_greater {
//    //        //    Coordinate split_point =
//    //        Coordinate(_rectangle.upper_bound_x(),
//    //        subtraction.y_high(_rectangle.upper_bound_x()));
//    //        //    Rectangle::RectangleSplit rectangle_split =
//    //        _rectangle.split(split_point);
//    //        //    fill_boundary_edge(_camera, rectangle_split.top_left(),
//    //        subtraction, Q1);
//    //        //    _camera->draw_rectangle(Color(GREEN, 64),
//    //        rectangle_split.top_right());
//    //        //    _camera->draw_rectangle(Color(GREEN, 64),
//    //        rectangle_split.bottom_right());
//    //        //    _camera->draw_rectangle(Color(GREEN, 64),
//    //        rectangle_split.bottom_left());
//    //        //} else {
//    //        //    _camera->draw_rectangle(Color(YELLOW, 64), _rectangle);
//    //        //}
//    //    } else if (d_angle < PI) {
//    //        ufloat x_ratio =
//    //        subtraction.center().distance(_rectangle.bottom_left()) /
//    //        subtraction.radius(); ufloat y_ratio =
//    //        subtraction.center().distance(_rectangle.top_right()) /
//    //        subtraction.radius();
//    //
//    //        if (x_ratio_greater && y_ratio_greater) {
//    //            Coordinate split_point =
//    //            Coordinate(subtraction.x_low(_rectangle.lower_bound_y()),
//    //            subtraction.y_high(_rectangle.upper_bound_x()));
//    //            Rectangle::RectangleSplit rectangle_split =
//    //            _rectangle.split(split_point); fill_boundary_full(_camera,
//    //            rectangle_split.top_right()); fill_boundary_full(_camera,
//    //            rectangle_split.bottom_left()); fill_boundary_full(_camera,
//    //            rectangle_split.top_left()); fill_boundary_edge(_camera,
//    //            rectangle_split.bottom_right(), source, Q2);
//    //        } else if (x_ratio_greater) {
//    //            Coordinate split_point =
//    //            Coordinate(subtraction.x_low(_rectangle.upper_bound_y()),
//    //            _rectangle.upper_bound_y()); Rectangle::RectangleSplit
//    //            rectangle_split = _rectangle.split(split_point);
//    //            fill_boundary_edge(_camera, rectangle_split.bottom_left(),
//    //            source, Q2);
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.top_right());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.bottom_right());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.top_left());
//    //        } else if (y_ratio_greater) {
//    //            Coordinate split_point =
//    //            Coordinate(_rectangle.lower_bound_x(),
//    //            subtraction.y_high(_rectangle.lower_bound_x()));
//    //            Rectangle::RectangleSplit rectangle_split =
//    //            _rectangle.split(split_point); fill_boundary_edge(_camera,
//    //            rectangle_split.top_right(), source, Q2);
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.bottom_right());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.bottom_left());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.top_left());
//    //        } else {
//    //            _camera->draw(Color(MAGENTA, 64), _rectangle);
//    //        }
//    //    } else if (d_angle < PI_3D2) {
//    //        ufloat x_ratio =
//    //        subtraction.center().distance(_rectangle.top_left()) /
//    //        subtraction.radius(); ufloat y_ratio =
//    //        subtraction.center().distance(_rectangle.bottom_right()) /
//    //        subtraction.radius();
//    //
//    //        if (x_ratio_greater && y_ratio_greater) {
//    //            Coordinate split_point =
//    //            Coordinate(subtraction.x_low(_rectangle.upper_bound_y()),
//    //            subtraction.center().y() -
//    //            (subtraction.y_high(_rectangle.upper_bound_x()) -
//    //            subtraction.center().y())); Rectangle::RectangleSplit
//    //            rectangle_split = _rectangle.split(split_point);
//    //            fill_boundary_full(_camera, rectangle_split.bottom_right());
//    //            fill_boundary_full(_camera, rectangle_split.bottom_left());
//    //            fill_boundary_full(_camera, rectangle_split.top_left());
//    //            fill_boundary_edge(_camera, rectangle_split.top_right(),
//    //            source, Q3);
//    //        } else if (x_ratio_greater) {
//    //            Coordinate split_point =
//    //            Coordinate(subtraction.x_low(_rectangle.lower_bound_y()),
//    //            _rectangle.lower_bound_y()); Rectangle::RectangleSplit
//    //            rectangle_split = _rectangle.split(split_point);
//    //            fill_boundary_edge(_camera, rectangle_split.top_left(),
//    //            source, Q3);
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.top_right());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.bottom_right());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.bottom_left());
//    //        } else if (y_ratio_greater) {
//    //            Coordinate split_point =
//    //            Coordinate(_rectangle.lower_bound_x(),
//    //            subtraction.y_low(_rectangle.lower_bound_x()));
//    //            Rectangle::RectangleSplit rectangle_split =
//    //            _rectangle.split(split_point); fill_boundary_edge(_camera,
//    //            rectangle_split.bottom_right(), source, Q3);
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.top_right());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.bottom_left());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.top_left());
//    //        } else {
//    //            _camera->draw(Color(MAGENTA, 64), _rectangle);
//    //        }
//    //    } else { // if (d_angle < PI_2) {
//    //        ufloat x_ratio =
//    //        subtraction.center().distance(_rectangle.top_right()) /
//    //        subtraction.radius(); ufloat y_ratio =
//    //        subtraction.center().distance(_rectangle.bottom_left()) /
//    //        subtraction.radius();
//    //
//    //        if (x_ratio_greater && y_ratio_greater) {
//    //            Coordinate split_point =
//    //            Coordinate(subtraction.x_high(_rectangle.upper_bound_y()),
//    //            subtraction.y_low(_rectangle.lower_bound_x()));
//    //            Rectangle::RectangleSplit rectangle_split =
//    //            _rectangle.split(split_point); fill_boundary_full(_camera,
//    //            rectangle_split.top_right()); fill_boundary_full(_camera,
//    //            rectangle_split.bottom_right()); fill_boundary_full(_camera,
//    //            rectangle_split.bottom_left()); fill_boundary_edge(_camera,
//    //            rectangle_split.top_left(), source, Q4);
//    //        } else if (x_ratio_greater) {
//    //            Coordinate split_point =
//    //            Coordinate(subtraction.x_high(_rectangle.lower_bound_y()),
//    //            _rectangle.lower_bound_y()); Rectangle::RectangleSplit
//    //            rectangle_split = _rectangle.split(split_point);
//    //            fill_boundary_edge(_camera, rectangle_split.top_right(),
//    //            source, Q4);
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.bottom_right());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.bottom_left());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.top_left());
//    //        } else if (y_ratio_greater) {
//    //            Coordinate split_point =
//    //            Coordinate(_rectangle.upper_bound_x(),
//    //            subtraction.y_low(_rectangle.upper_bound_x()));
//    //            Rectangle::RectangleSplit rectangle_split =
//    //            _rectangle.split(split_point); fill_boundary_edge(_camera,
//    //            rectangle_split.bottom_left(), source, Q4);
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.top_right());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.bottom_right());
//    //            //_camera->draw_rectangle(Color(GREEN, 64),
//    //            rectangle_split.top_left());
//    //        } else {
//    //            _camera->draw(Color(MAGENTA, 64), _rectangle);
//    //        }
//    //    }
//    //}
//    //
//    // else if (updated_sources.size() > 1) {
//    //    _camera->draw(Color(RED, 64), _rectangle);
//    //}
//
//    return *this;
//}
