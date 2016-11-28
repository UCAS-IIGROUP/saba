﻿#include "GLMMDModel.h"
#include <Saba/GL/GLVertexUtil.h>
#include <Saba/GL/GLTextureUtil.h>
#include <Saba/Base/Log.h>

namespace saba
{
	GLMMDModel::GLMMDModel()
	{
	}

	GLMMDModel::~GLMMDModel()
	{
	}

	bool GLMMDModel::Create(std::shared_ptr<MMDModel> mmdModel)
	{
		Destroy();

		size_t vtxCount = mmdModel->GetVertexCount();
		auto positions = mmdModel->GetPositions();
		auto normals = mmdModel->GetNormals();
		auto uvs = mmdModel->GetUVs();
		m_posVBO = CreateVBO(positions, vtxCount, GL_DYNAMIC_DRAW);
		m_norVBO = CreateVBO(normals, vtxCount, GL_DYNAMIC_DRAW);
		m_uvVBO = CreateVBO(uvs, vtxCount, GL_DYNAMIC_DRAW);

		m_posBinder = MakeVertexBinder<glm::vec3>();
		m_norBinder = MakeVertexBinder<glm::vec3>();
		m_uvBinder = MakeVertexBinder<glm::vec2>();

		const void* iboBuf = mmdModel->GetIndices();
		size_t indexCount = mmdModel->GetIndexCount();
		size_t indexElemSize = mmdModel->GetIndexElementSize();
		switch (indexElemSize)
		{
		case 1:
			m_ibo = CreateIBO((uint8_t*)iboBuf, indexCount, GL_STATIC_DRAW);
			m_indexType = GL_UNSIGNED_BYTE;
			m_indexTypeSize = 1;
			break;
		case 2:
			m_ibo = CreateIBO((uint16_t*)iboBuf, indexCount, GL_STATIC_DRAW);
			m_indexType = GL_UNSIGNED_SHORT;
			m_indexTypeSize = 2;
			break;
		case 4:
			m_ibo = CreateIBO((uint32_t*)iboBuf, indexCount, GL_STATIC_DRAW);
			m_indexType = GL_UNSIGNED_INT;
			m_indexTypeSize = 4;
			break;
		default:
			SABA_ERROR("Unknown Index Size. [{}]", indexElemSize);
			return false;
		}

		// Material
		size_t matCount = mmdModel->GetMaterialCount();
		auto materials = mmdModel->GetMaterials();
		m_materials.resize(matCount);
		for (size_t matIdx = 0; matIdx < matCount; matIdx++)
		{
			auto& dest = m_materials[matIdx];
			const auto& src = materials[matIdx];

			dest.m_diffuse = src.m_diffuse;
			dest.m_alpha = src.m_alpha;
			dest.m_specularPower = src.m_specularPower;
			dest.m_specular = src.m_specular;
			dest.m_ambient = src.m_ambient;
			dest.m_edgeFlag = src.m_edgeFlag;
			if (!src.m_texture.empty())
			{
				dest.m_texture = CreateTextureFromFile(src.m_texture, true, true);
			}
			if (!src.m_spTexture.empty())
			{
				dest.m_spTexture = CreateTextureFromFile(src.m_spTexture);
			}
			dest.m_spTextureMode = src.m_spTextureMode;
			if (!src.m_toonTexture.empty())
			{
				dest.m_toonTexture = CreateTextureFromFile(src.m_toonTexture);
			}
		}

		// SubMesh
		size_t subMeshCount = mmdModel->GetSubMeshCount();
		auto subMeshes = mmdModel->GetSubMeshes();
		m_subMeshes.resize(subMeshCount);
		for (size_t subMeshIdx = 0; subMeshIdx < subMeshCount; subMeshIdx++)
		{
			auto& dest = m_subMeshes[subMeshIdx];
			const auto& src = subMeshes[subMeshIdx];

			dest.m_beginIndex = src.m_beginIndex;
			dest.m_vertexCount = src.m_vertexCount;
			dest.m_materialID = src.m_materialID;
		}

		return true;
	}

	void GLMMDModel::Destroy()
	{
	}

}