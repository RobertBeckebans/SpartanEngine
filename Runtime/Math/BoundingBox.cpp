/*
Copyright(c) 2016-2020 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES ===========
#include "BoundingBox.h"
#include "Matrix.h"
#include <vector>
//======================

namespace Spartan::Math
{
	const BoundingBox BoundingBox::Zero(Vector3::Zero, Vector3::Zero);

	BoundingBox::BoundingBox()
	{
		m_min = Vector3::Infinity;
		m_max = Vector3::InfinityNeg;
	}

	BoundingBox::BoundingBox(const Vector3& min, const Vector3& max)
	{
		this->m_min = min;
		this->m_max = max;
	}

    BoundingBox::BoundingBox(const std::vector<Vector3>& points)
    {
        m_min = Vector3::Infinity;
        m_max = Vector3::InfinityNeg;

        for (const auto& point : points)
        {
            m_max.x = Max(m_max.x, point.x);
            m_max.y = Max(m_max.y, point.y);
            m_max.z = Max(m_max.z, point.z);

            m_min.x = Min(m_min.x, point.x);
            m_min.y = Min(m_min.y, point.y);
            m_min.z = Min(m_min.z, point.z);
        }
    }

	BoundingBox::BoundingBox(const std::vector<RHI_Vertex_PosTexNorTan>& vertices)
	{
		m_min = Vector3::Infinity;
		m_max = Vector3::InfinityNeg;

		for (const auto& vertex : vertices)
		{
			m_max.x = Max(m_max.x, vertex.pos[0]);
			m_max.y = Max(m_max.y, vertex.pos[1]);
			m_max.z = Max(m_max.z, vertex.pos[2]);

			m_min.x = Min(m_min.x, vertex.pos[0]);
			m_min.y = Min(m_min.y, vertex.pos[1]);
			m_min.z = Min(m_min.z, vertex.pos[2]);
		}
	}

	Intersection BoundingBox::IsInside(const Vector3& point) const
	{
		if (point.x < m_min.x || point.x > m_max.x ||
			point.y < m_min.y || point.y > m_max.y ||
			point.z < m_min.z || point.z > m_max.z)
		{
			return Outside;
		}
		else
		{
			return Inside;
		}
	}

	Intersection BoundingBox::IsInside(const BoundingBox& box) const
	{
		if (box.m_max.x < m_min.x || box.m_min.x > m_max.x ||
			box.m_max.y < m_min.y || box.m_min.y > m_max.y ||
			box.m_max.z < m_min.z || box.m_min.z > m_max.z)
		{
			return Outside;
		}
		else if (
				box.m_min.x < m_min.x || box.m_max.x > m_max.x ||
				box.m_min.y < m_min.y || box.m_max.y > m_max.y ||
				box.m_min.z < m_min.z || box.m_max.z > m_max.z)
		{
			return Intersects;
		}
		else
		{
			return Inside;
		}
	}

	BoundingBox BoundingBox::Transform(const Matrix& transform) const
	{
        const Vector3 center_new = transform * GetCenter();
        const Vector3 extent_old = GetExtents();
        const Vector3 extend_new = Vector3
		(
			Abs(transform.m00) * extent_old.x + Abs(transform.m10) * extent_old.y + Abs(transform.m20) * extent_old.z,
			Abs(transform.m01) * extent_old.x + Abs(transform.m11) * extent_old.y + Abs(transform.m21) * extent_old.z,
			Abs(transform.m02) * extent_old.x + Abs(transform.m12) * extent_old.y + Abs(transform.m22) * extent_old.z
		);

		return BoundingBox(center_new - extend_new, center_new + extend_new);
	}

    void BoundingBox::Merge(const BoundingBox& box)
    {
        m_min.x = Min(m_min.x, box.m_min.x);
        m_min.y = Min(m_min.y, box.m_min.y);
        m_min.z = Min(m_min.z, box.m_min.z);
        m_max.x = Max(m_max.x, box.m_max.x);
        m_max.y = Max(m_max.x, box.m_max.x);
        m_max.z = Max(m_max.x, box.m_max.x);
    }
}
