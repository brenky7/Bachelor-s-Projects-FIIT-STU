-- ---------------------------------------------------------------------------------- --
-- ------------------ Procedure for Proces 2: Inserting an exemplar ----------------- --
-- ---------------------------------------------------------------------------------- --


CREATE OR REPLACE PROCEDURE insert_exemplar(
    -- Input params 
    p_name VARCHAR(50),
    p_description VARCHAR(500),
    p_owner_id BIGINT,
    p_category_id INT,
    p_storage_location_id INT DEFAULT NULL,
	p_state exemplar_status DEFAULT 'stored'
)
AS $$
DECLARE
    v_owner_exists BOOLEAN;
    v_category_exists BOOLEAN;
    v_storage_location_exists BOOLEAN := TRUE; -- Assume true if p_storage_location_id is NULL
BEGIN
    
	-- Check if the owner ID exists in the partners table
    SELECT EXISTS (SELECT 1 FROM partners WHERE id = p_owner_id) INTO v_owner_exists;
    IF NOT v_owner_exists THEN
        -- If the owner ID does not exist, raise notice and return
        RAISE NOTICE 'Error: Owner ID does not exist.';
        RETURN;
    END IF;

    -- Check if the category ID exists in the category table
    SELECT EXISTS (SELECT 1 FROM category WHERE id = p_category_id) INTO v_category_exists;
    IF NOT v_category_exists THEN
        -- If the category ID does not exist, raise notice and return
        RAISE NOTICE 'Error: Category ID does not exist.';
        RETURN;
    END IF;

    -- If storage_location_id is not NULL, check if the storage location ID exists in the storage_positions table
    IF p_storage_location_id IS NOT NULL THEN
        SELECT EXISTS (SELECT 1 FROM storage_positions WHERE id = p_storage_location_id) INTO v_storage_location_exists;
        IF NOT v_storage_location_exists THEN
            -- If the storage location ID does not exist, raise notice and return
            RAISE NOTICE 'Error: Storage Location ID does not exist.';
            RETURN;
        END IF;
    END IF;

    -- Insert the exemplar details into the exemplars table
    BEGIN
        INSERT INTO exemplars (name, description, owner_id, category_id, storage_location, state)
        VALUES (p_name, p_description, p_owner_id, p_category_id, p_storage_location_id, p_state);
    EXCEPTION
        WHEN others THEN
            RAISE NOTICE 'Error: Failed to insert exemplar details.';
            RETURN;
    END;
END;
$$ LANGUAGE plpgsql;



-- ---------------------------------------------------------------------------------- --
-- ---------------------------------- Example calls --------------------------------- --
-- ---------------------------------------------------------------------------------- --


CALL insert_exemplar(
    'Exemplar 9', -- p_name
    'Description 9', -- p_description
    1, -- p_owner_id 
    1, -- p_category_id 
    1 -- p_storage_location_id 
);