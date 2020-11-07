#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"

#include "hittable.h"


class moving_sphere : public hittable {
  public:
    moving_sphere() {}
    moving_sphere(
        point3 ctr0, point3 ctr1, double r, interval sphere_period, shared_ptr<material> m)
      : center0(ctr0), center1(ctr1), radius(r), period(sphere_period), mat_ptr(m)
    {};

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    virtual bool bounding_box(double time_start, double time_end, aabb& output_box)
        const override;

    point3 center(double time) const;

  public:
    point3 center0, center1;
    interval period;
    double radius;
    shared_ptr<material> mat_ptr;
};


point3 moving_sphere::center(double time) const{
    auto t = interval(0,1).clamp(period.fraction(time));
    return lerp(center0, center1, t);
}


bool moving_sphere::bounding_box(double time_start, double time_end, aabb& output_box) const {
    aabb box0(
        center(time_start) - vec3(radius, radius, radius),
        center(time_start) + vec3(radius, radius, radius));
    aabb box1(
        center(time_end) - vec3(radius, radius, radius),
        center(time_end) + vec3(radius, radius, radius));
    output_box = surrounding_box(box0, box1);
    return true;
}


bool moving_sphere::hit(const ray& r, interval ray_t, hit_record& rec) const {
    vec3 oc = r.origin() - center(r.time());
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (!ray_t.contains(root)) {
        root = (-half_b + sqrtd) / a;
        if (!ray_t.contains(root))
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center(r.time())) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}


#endif
